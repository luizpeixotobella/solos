import http from 'node:http';
import fs from 'node:fs/promises';
import path from 'node:path';
import crypto from 'node:crypto';
import { pathToFileURL } from 'node:url';
import { createLab, root } from './lab.mjs';

export async function createDemoServer({ port = 0 } = {}) {
  const lab = await createLab();
  const token = crypto.randomBytes(32).toString('hex');
  const nonce = crypto.randomBytes(24).toString('base64');
  const template = await fs.readFile(path.join(root, 'ui/index.html'), 'utf8');
  let runningAction = false;
  let origin;
  const server = http.createServer(async (request, response) => {
    const send = (status, payload, contentType = 'application/json') => {
      response.writeHead(status, {
        'Content-Type': contentType, 'Cache-Control': 'no-store',
        'X-Content-Type-Options': 'nosniff', 'Referrer-Policy': 'no-referrer',
        'Content-Security-Policy': `default-src 'none'; script-src 'nonce-${nonce}'; style-src 'nonce-${nonce}'; connect-src 'self'; base-uri 'none'; form-action 'none'; frame-ancestors 'none'`,
      });
      response.end(contentType === 'application/json' ? JSON.stringify(payload) : payload);
    };
    // Restrict Host as well as bind address: no DNS rebinding or cross-origin action.
    if (request.headers.host !== new URL(origin).host) return send(403, { error: 'local_host_required' });
    try {
      if (request.method === 'GET' && request.url === '/') {
        return send(200, template.replaceAll('__NONCE__', nonce).replace('__TOKEN__', token), 'text/html; charset=utf-8');
      }
      if (request.method === 'GET' && request.url === '/api/state') return send(200, await lab.snapshot());
      if (request.method !== 'POST' || request.url !== '/api/action') return send(404, { error: 'not_found' });
      if (request.headers.origin !== origin || request.headers['x-demo-token'] !== token) {
        return send(403, { error: 'local_session_required' });
      }
      if (runningAction) return send(409, { error: 'action_in_progress' });
      runningAction = true;
      try {
        let text = '';
        for await (const chunk of request) {
          text += chunk;
          if (Buffer.byteLength(text) > 1024) return send(413, { error: 'payload_too_large' });
        }
        const data = JSON.parse(text);
        const reference = 'demo-support-001';
        if (data.action === 'pay') {
          if (![2500, 10000, 50000].includes(data.cents)) return send(400, { error: 'invalid_demo_tier' });
          await lab.simulatePayment(reference, data.cents);
        } else if (data.action === 'claim') await lab.claim(reference);
        else if (data.action === 'refund') await lab.refund(reference);
        else return send(400, { error: 'invalid_demo_action' });
        return send(200, await lab.snapshot());
      } finally { runningAction = false; }
    } catch (error) {
      // Do not return execution internals, signed transaction material or private keys.
      const known = new Set(['no_settled_patrono_support', 'no_settled_support',
        'reference_amount_conflict', 'payment_already_refunded']);
      return send(400, { error: known.has(error.message) ? error.message : 'demo_action_rejected' });
    }
  });
  try {
    await new Promise((resolve, reject) => { server.once('error', reject); server.listen(port, '127.0.0.1', resolve); });
    origin = `http://127.0.0.1:${server.address().port}`;
    return { url: origin, lab, async close() {
      await new Promise((resolve, reject) => server.close((e) => e ? reject(e) : resolve()));
      await lab.close();
    } };
  } catch (error) { await lab.close(); throw error; }
}

if (process.argv[1] && import.meta.url === pathToFileURL(process.argv[1]).href) {
  const app = await createDemoServer({ port: Number(process.env.SOLOS_DEMO_PORT ?? 8766) });
  console.log(`SolOS Heart Pass LAB — somente local: ${app.url}`);
  console.log('Nenhum pagamento real. Ao fechar, carteiras/chaves e blockchain de teste desaparecem.');
  for (const signal of ['SIGINT', 'SIGTERM']) process.once(signal, async () => { await app.close(); process.exit(0); });
}
