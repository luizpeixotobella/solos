import test from 'node:test';
import assert from 'node:assert/strict';
import { id } from 'ethers';
import { createLab, tierForCents } from '../src/lab.mjs';

test('tiers preserve smaller supports and propose a pass only for Patrono', () => {
  assert.deepEqual([2500, 10000, 50000].map(tierForCents).map((t) => [t.credits, t.pass]),
    [[10, false], [50, false], [250, true]]);
  for (const value of [2499, NaN, Infinity, 50000.1]) assert.throws(() => tierForCents(value));
});

test('Safe, settled support, owner signatures, duplicate protection and refund lifecycle', async (t) => {
  const lab = await createLab();
  try {
    await t.test('real Safe contract in a local EVM; not an address-only wallet', async () => {
      const s = await lab.snapshot();
      assert.equal(s.safeVersion, '1.5.0');
      assert.notEqual(s.wallet, s.owner);
      assert.notEqual(await lab.provider.getCode(lab.wallet), '0x');
      assert.deepEqual([...(await lab.safe.getOwners())], [lab.owner.address]);
      assert.equal(s.threshold, 1);
    });
    await t.test('pending support grants neither credits nor pass', async () => {
      await lab.simulatePayment('demo-pending', 50000, { settled: false });
      await assert.rejects(lab.claim('demo-pending'), /no_settled/);
      assert.equal((await lab.snapshot()).credits, 0);
      await assert.rejects(lab.pass.claim(id('demo-pending')));
    });
    await t.test('smaller support earns credits, not a pass', async () => {
      await lab.simulatePayment('demo-small', 2500);
      await assert.rejects(lab.claim('demo-small'), /no_settled/);
      assert.equal((await lab.snapshot()).credits, 10);
    });
    await t.test('settlement and replay grant exactly once; conflicting values fail', async () => {
      await lab.simulatePayment('demo-pending', 50000);
      assert.equal((await lab.snapshot()).credits, 260);
      assert.deepEqual(await lab.simulatePayment('demo-pending', 50000), { duplicate: true });
      assert.equal((await lab.snapshot()).credits, 260);
      await assert.rejects(lab.simulatePayment('demo-pending', 10000), /conflict/);
    });
    await t.test('unauthorized issuer and wrong claiming wallet are rejected on chain', async () => {
      const alien = await lab.provider.getSigner(2);
      await assert.rejects(lab.pass.connect(alien).approveSupport(id('demo-forged'), lab.wallet));
      await assert.rejects(lab.pass.connect(alien).claim(id('demo-pending')));
    });
    const claimData = lab.pass.interface.encodeFunctionData('claim', [id('demo-pending')]);
    await t.test('wrong Safe-owner signature is rejected', async () => {
      await assert.rejects(lab.executePrepared(await lab.prepareSafeCall(claimData, lab.stranger)));
      assert.equal((await lab.snapshot()).passBalance, 0);
    });
    let signed;
    await t.test('owner-signed Safe call receives one personal ERC1155 pass', async () => {
      signed = await lab.prepareSafeCall(claimData);
      await lab.executePrepared(signed);
      assert.equal((await lab.snapshot()).passBalance, 1);
    });
    await t.test('signature replay and repeated claim cannot mint again', async () => {
      await assert.rejects(lab.executePrepared(signed));
      await assert.rejects(lab.claim('demo-pending'));
      assert.equal((await lab.snapshot()).passBalance, 1);
    });
    await t.test('personal pass cannot be transferred out of the smart wallet', async () => {
      const data = lab.pass.interface.encodeFunctionData('safeTransferFrom',
        [lab.wallet, lab.stranger.address, 1, 1, '0x']);
      await assert.rejects(lab.executePrepared(await lab.prepareSafeCall(data)));
      assert.equal((await lab.snapshot()).passBalance, 1);
    });
    await t.test('second support does not duplicate the personal pass', async () => {
      await lab.simulatePayment('demo-second', 50000);
      await lab.claim('demo-second');
      assert.equal((await lab.snapshot()).passBalance, 1);
      assert.equal(Number(await lab.pass.activeClaims(lab.wallet)), 2);
    });
    await t.test('refund of one support preserves another active entitlement', async () => {
      await lab.refund('demo-pending');
      assert.equal((await lab.snapshot()).passBalance, 1);
      await assert.rejects(lab.simulatePayment('demo-pending', 50000), /already_refunded/);
    });
    await t.test('last refund revokes access, reverses unspent credits and stays terminal', async () => {
      await lab.refund('demo-second');
      assert.equal((await lab.snapshot()).passBalance, 0);
      assert.equal((await lab.snapshot()).credits, 10);
      await assert.rejects(lab.claim('demo-second'));
      await assert.rejects(lab.refund('demo-second'));
    });
  } finally { await lab.close(); }
});
