// Isolated OpenClaw SDK bridge. Prompt travels through stdin, never a shell or
// an oversized execve argument. Uses the same modelRun contract as infer CLI.
import { realpath } from 'node:fs/promises';
import { dirname, join } from 'node:path';
import { pathToFileURL } from 'node:url';
import { randomUUID } from 'node:crypto';

try {
  const [binary, agentId, modelRef] = process.argv.slice(2);
  if (!/^[a-zA-Z0-9_-]+$/.test(agentId || '') || !modelRef?.startsWith('openai/')) throw new Error('Invalid inference owner/model');
  const parts = []; let length = 0;
  for await (const part of process.stdin) { length += part.length; if (length > 900000) throw new Error('Prompt too large'); parts.push(part); }
  const prompt = Buffer.concat(parts).toString('utf8');
  const sdk = pathToFileURL(join(dirname(await realpath(binary)), 'dist/plugin-sdk/gateway-runtime.js'));
  const { callGatewayFromCli } = await import(sdk.href);
  const sessionId = `model-run-${randomUUID()}`;
  const response = await callGatewayFromCli('agent', { json: true, timeout: '170000', expectFinal: true }, {
    agentId, sessionId, sessionKey: `agent:${agentId}:explicit:${sessionId}`, message: prompt,
    provider: 'openai', model: modelRef.slice('openai/'.length), modelRun: true,
    promptMode: 'none', cleanupBundleMcpOnRunEnd: true, idempotencyKey: randomUUID()
  }, { clientName: 'gateway-client', mode: 'backend', scopes: ['operator.admin'], progress: false });
  if (response?.status !== 'ok') throw new Error('Inference request failed');
  process.stdout.write(JSON.stringify({ ok: true, outputs: response?.result?.payloads || [] }));
} catch {
  process.stderr.write('OpenClaw inference bridge failed.\n'); process.exitCode = 1;
}
