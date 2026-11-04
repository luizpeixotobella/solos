import fs from 'node:fs/promises';
import path from 'node:path';
import { createLab, root } from './lab.mjs';

const lab = await createLab();
try {
  await lab.simulatePayment('demo-patrono-001', 50000);
  await lab.claim('demo-patrono-001');
  const claimed = await lab.snapshot();
  await lab.refund('demo-patrono-001');
  const refunded = await lab.snapshot();
  const result = { claimed, refunded };
  await fs.mkdir(path.join(root, 'artifacts'), { recursive: true });
  await fs.writeFile(path.join(root, 'artifacts/demo.json'), JSON.stringify(result, null, 2));
  console.log(JSON.stringify({ mode: claimed.mode, chainId: claimed.chainId, safeVersion: claimed.safeVersion,
    afterClaim: { pass: claimed.passBalance, credits: claimed.credits },
    afterRefund: { pass: refunded.passBalance, credits: refunded.credits },
    receipt: 'artifacts/demo.json', realMoney: false }, null, 2));
} finally { await lab.close(); }
