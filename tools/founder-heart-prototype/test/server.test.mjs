import test from 'node:test';
import assert from 'node:assert/strict';
import http from 'node:http';
import { createDemoServer } from '../src/server.mjs';

test('loopback UI, session/origin checks and complete synthetic payment lifecycle', async () => {
  const app = await createDemoServer();
  try {
    const home = await fetch(app.url);
    assert.equal(home.status, 200);
    assert.match(home.headers.get('content-security-policy'), /frame-ancestors 'none'/);
    const html = await home.text();
    assert.match(html, /SEM DINHEIRO REAL/);
    const token = html.match(/name="demo-session" content="([^"]+)"/)[1];
    const post = (action, headers = {}, cents = 50000) => fetch(app.url + '/api/action', {
      method: 'POST', headers: { 'Content-Type': 'application/json', ...headers },
      body: JSON.stringify({ action, cents }),
    });
    assert.equal((await post('pay')).status, 403);
    assert.equal((await post('pay', { Origin: 'https://example.org', 'X-Demo-Token': token })).status, 403);
    const reboundStatus = await new Promise((resolve, reject) => {
      http.get(app.url + '/api/state', { headers: { Host: 'example.org' } }, (r) => {
        r.resume(); resolve(r.statusCode);
      }).on('error', reject);
    });
    assert.equal(reboundStatus, 403);
    const headers = { Origin: app.url, 'X-Demo-Token': token };
    assert.equal((await post('pay', headers, 50123)).status, 400);
    assert.equal((await post('pay', headers)).status, 200);
    const duplicate = await post('pay', headers);
    assert.equal((await duplicate.json()).credits, 250);
    const claimed = await post('claim', headers);
    const s = await claimed.json();
    assert.equal(claimed.status, 200);
    assert.equal(s.passBalance, 1);
    assert.equal(s.credits, 250);
    assert.equal(s.realMoney, false);
    assert.equal(s.chainId, 31337);
    assert.equal(JSON.stringify(s).includes('privateKey'), false);
    assert.equal((await post('claim', headers)).status, 400);
    const refund = await post('refund', headers);
    const r = await refund.json();
    assert.equal(r.passBalance, 0);
    assert.equal(r.credits, 0);
    assert.equal((await post('pay', headers)).status, 400);
    assert.equal((await post('refund', headers)).status, 400);
  } finally { await app.close(); }
});
