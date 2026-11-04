import fs from 'node:fs';
import path from 'node:path';
import { createRequire } from 'node:module';
import { fileURLToPath } from 'node:url';
import { createHardhatRuntimeEnvironment } from 'hardhat/hre';
import solc from 'solc';
import { BrowserProvider, Contract, ContractFactory, Wallet, ZeroAddress, id } from 'ethers';

const require = createRequire(import.meta.url);
export const root = fileURLToPath(new URL('../', import.meta.url));
const safeRoot = path.dirname(require.resolve('@safe-global/safe-smart-account/package.json'));
const artifact = (name, location) => JSON.parse(fs.readFileSync(path.join(safeRoot,
  'build/artifacts/contracts', location, `${name}.json`), 'utf8'));
let compiled;

function compilePass() {
  if (compiled) return compiled;
  const input = {
    language: 'Solidity',
    sources: { 'FounderHeartPass.sol': { content: fs.readFileSync(path.join(root, 'contracts/FounderHeartPass.sol'), 'utf8') } },
    settings: { optimizer: { enabled: true, runs: 200 }, evmVersion: 'cancun',
      outputSelection: { '*': { '*': ['abi', 'evm.bytecode.object'] } } },
  };
  const output = JSON.parse(solc.compile(JSON.stringify(input), { import: (name) => {
    try { return { contents: fs.readFileSync(require.resolve(name), 'utf8') }; }
    catch { return { error: `Import not found: ${name}` }; }
  } }));
  const errors = (output.errors ?? []).filter((e) => e.severity === 'error');
  if (errors.length) throw new Error(errors.map((e) => e.formattedMessage).join('\n'));
  const c = output.contracts['FounderHeartPass.sol'].FounderHeartPass;
  compiled = { abi: c.abi, bytecode: `0x${c.evm.bytecode.object}` };
  return compiled;
}

export function tierForCents(cents) {
  if (!Number.isSafeInteger(cents) || cents < 2500) throw new Error('invalid_support_amount');
  if (cents >= 50000) return { code: 'patrono', credits: 250, pass: true };
  if (cents >= 10000) return { code: 'founder_heart', credits: 50, pass: false };
  return { code: 'apoiador', credits: 10, pass: false };
}

/** No external RPC, production credentials, fixed seed or persisted signing keys. */
export async function createLab() {
  const temporaryKeys = Array.from({ length: 4 }, () => Wallet.createRandom());
  const hre = await createHardhatRuntimeEnvironment({
    networks: { local: { type: 'edr-simulated', chainId: 31337, hardfork: 'cancun',
      accounts: temporaryKeys.map((w) => ({ privateKey: w.privateKey, balance: '100000000000000000000' })) } },
  }, {}, root);
  const connection = await hre.network.create('local');
  const evm = connection.provider;
  const provider = new BrowserProvider(evm, undefined, { cacheTimeout: -1 });
  provider.pollingInterval = 10;
  const issuer = await provider.getSigner(0);
  const owner = temporaryKeys[1];
  const stranger = temporaryKeys[2];
  const deploy = async (a, args = []) => {
    const c = await new ContractFactory(a.abi, a.bytecode, issuer).deploy(...args);
    await c.waitForDeployment();
    return c;
  };
  try {
    const singletonArtifact = artifact('Safe', 'Safe.sol');
    const singleton = await deploy(singletonArtifact);
    const handler = await deploy(artifact('CompatibilityFallbackHandler', 'handler/CompatibilityFallbackHandler.sol'));
    const factory = await deploy(artifact('SafeProxyFactory', 'proxies/SafeProxyFactory.sol'));
    const initializer = singleton.interface.encodeFunctionData('setup', [
      [owner.address], 1, ZeroAddress, '0x', await handler.getAddress(), ZeroAddress, 0, ZeroAddress,
    ]);
    const receipt = await (await factory.createProxyWithNonce(await singleton.getAddress(), initializer, 1)).wait();
    const log = receipt.logs.map((l) => { try { return factory.interface.parseLog(l); } catch { return null; } })
      .find((l) => l?.name === 'ProxyCreation');
    const safe = new Contract(log.args.proxy, singletonArtifact.abi, issuer);
    const pass = await deploy(compilePass(), [await issuer.getAddress()]);
    const wallet = await safe.getAddress();
    const payments = new Map();
    const events = [];
    let balance = 0;

    const record = (kind, ref, hash = null) => events.push({ kind, reference: ref, transaction: hash });
    async function prepareSafeCall(data, signer = owner, to = null) {
      const destination = to ?? await pass.getAddress();
      const nonce = await safe.nonce();
      const args = [destination, 0, data, 0, 0, 0, 0, ZeroAddress, ZeroAddress];
      const hash = await safe.getTransactionHash(...args, nonce);
      return { args, signature: signer.signingKey.sign(hash).serialized, nonce: Number(nonce) };
    }
    async function executePrepared(tx) {
      const r = await (await safe.execTransaction(...tx.args, tx.signature)).wait();
      const success = r.logs.map((l) => { try { return safe.interface.parseLog(l); } catch { return null; } })
        .some((l) => l?.name === 'ExecutionSuccess');
      if (!success) throw new Error('safe_execution_failed');
      return r;
    }

    return {
      provider, issuer, safe, pass, wallet, owner, stranger, payments, events,
      prepareSafeCall, executePrepared,
      async simulatePayment(reference, cents, { settled = true } = {}) {
        // Synthetic references only: never import Stripe/PIX IDs or personal information.
        if (!/^demo-[a-z0-9-]{1,64}$/.test(reference)) throw new Error('demo_reference_required');
        const tier = tierForCents(cents);
        const old = payments.get(reference);
        if (old) {
          if (old.cents !== cents) throw new Error('reference_amount_conflict');
          if (old.status === 'refunded') throw new Error('payment_already_refunded');
          if (old.status === 'settled' || !settled) return { duplicate: true };
        }
        if (!settled) {
          payments.set(reference, { reference, cents, ...tier, status: 'pending', onchain: null });
          record('pagamento fictício pendente', reference);
          return { pending: true };
        }
        let hash = null;
        if (tier.pass) {
          const r = await (await pass.approveSupport(id(reference), wallet)).wait();
          hash = r.hash;
        }
        payments.set(reference, { reference, cents, ...tier, status: 'settled', onchain: hash });
        balance += tier.credits;
        record('apoio fictício confirmado', reference, hash);
        return { credits: tier.credits, passEligible: tier.pass };
      },
      async claim(reference) {
        const p = payments.get(reference);
        if (!p || p.status !== 'settled' || !p.pass) throw new Error('no_settled_patrono_support');
        const r = await executePrepared(await prepareSafeCall(pass.interface.encodeFunctionData('claim', [id(reference)])));
        record('passe resgatado com assinatura da Safe', reference, r.hash);
        return r.hash;
      },
      async refund(reference) {
        const p = payments.get(reference);
        if (!p || p.status !== 'settled') throw new Error('no_settled_support');
        let hash = null;
        if (p.pass) hash = (await (await pass.revokeSupport(id(reference))).wait()).hash;
        p.status = 'refunded';
        // No credit spend exists in this prototype; all granted credits remain reversible.
        balance -= p.credits;
        record('estorno fictício: acesso revogado e créditos revertidos', reference, hash);
      },
      async snapshot() {
        return {
          mode: 'local-prototype', chainId: 31337, realMoney: false,
          keys: 'efêmeras em memória; não são uma carteira pessoal',
          safeVersion: await safe.VERSION(), wallet, owner: owner.address,
          threshold: Number(await safe.getThreshold()), contract: await pass.getAddress(),
          passBalance: Number(await pass.balanceOf(wallet, 1)), credits: balance,
          nonce: Number(await safe.nonce()), payments: [...payments.values()], events: [...events],
          proposedTerms: 'Patrono: 250 créditos + 1 passe pessoal de acesso. Proposta, não termos públicos vigentes.',
          notImplemented: ['pagamentos reais', 'Polygon/testnet', 'recuperação/passkeys',
            'ERC-4337/bundler/paymaster', 'consultas patrocinadas', 'integração com a ISO'],
        };
      },
      async close() { provider.destroy(); await connection.close(); },
    };
  } catch (error) { provider.destroy(); await connection.close(); throw error; }
}
