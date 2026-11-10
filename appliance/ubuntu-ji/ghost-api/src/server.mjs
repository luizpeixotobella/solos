import { createServer } from 'node:http';
import { createHash, randomUUID, timingSafeEqual } from 'node:crypto';
import { readFile } from 'node:fs/promises';
import { realpathSync } from 'node:fs';
import { fileURLToPath } from 'node:url';
import { resolve } from 'node:path';
import { ApiError, completion, streamChunks, validateRequest } from './protocol.mjs';
import { createInference, runProcess } from './inference.mjs';
import { loadKnowledge, retrieve } from './knowledge.mjs';
import { evidenceQuery, boundEvidence } from './context.mjs';
import { applyContextProfile, latestUserText } from './openclaw-profile.mjs';

const digest = text => createHash('sha256').update(text).digest();
const json = (res, status, body) => { res.writeHead(status, { 'Content-Type': 'application/json; charset=utf-8', 'Cache-Control': 'no-store', 'X-Content-Type-Options': 'nosniff' }); res.end(JSON.stringify(body)); };

async function readBody(req, maxBytes) {
  let bytes = 0; const parts = [];
  for await (const part of req) { bytes += part.length; if (bytes > maxBytes) throw new ApiError(413, 'Request exceeds size limit'); parts.push(part); }
  try { return JSON.parse(Buffer.concat(parts).toString('utf8')); } catch { throw new ApiError(400, 'Malformed JSON'); }
}

export function createGhostServer({ config, documents = [], infer, classify, log = () => {} }) {
  if (!config.apiKey || config.apiKey.length < 32) throw new Error('Ghost API key must contain at least 32 characters');
  let active = 0; let rateStart = Date.now(); let requests = 0;
  const keyHash = digest(`Bearer ${config.apiKey}`);
  return createServer({ requestTimeout: 30000, headersTimeout: 15000, maxHeaderSize: 16384 }, async (req, res) => {
    const started = Date.now(); const requestId = randomUUID();
    res.setHeader('X-Request-Id', requestId);
    const path = req.url?.split('?')[0];
    let slot = false;
    let controller;
    let timer;
    let progressTimer;
    let progressFrame;
    let progressCount = 0;
    try {
      if (req.method === 'GET' && path === '/healthz') return json(res, 200, { status: 'alive', service: 'ghost', version: '0.2.1' });
      if (!timingSafeEqual(keyHash, digest(req.headers.authorization || ''))) throw new ApiError(401, 'Authentication required', 'invalid_api_key');
      if (req.method === 'GET' && path === '/v1/models') return json(res, 200, { object: 'list', data: [{ id: config.modelId, object: 'model', created: 1792886400, owned_by: 'lbartes' }] });
      if (req.method === 'GET' && path === '/v1/ghost/status') return json(res, 200, { schema: 'ghost.service.v1', version: '0.2.1', model: config.modelId, backend: config.backend, upstreamModel: config.upstreamModel,
        activeRequests: active, knowledgeDocuments: documents.length, contextProfiles: ['preserve', 'openclaw-lean'], capabilities: { text: true, tools: 'caller-executed; CLI backend uses validated structured proposals', stream: 'buffered model SSE; automatic waiting notices for opted-in interactive clients, not token-by-token model output', waitingNoticeIntervalMs: config.progressIntervalMs ?? 240000, personalMemory: false, webSearch: false, hostExecution: false, policyClassifier: Boolean(classify) }, foundationModelTrainedHere: false });
      if (req.method !== 'POST' || !['/v1/chat/completions', '/v1/ghost/classify'].includes(path)) throw new ApiError(404, 'Route not found', 'not_found');
      if (!req.headers['content-type']?.toLowerCase().startsWith('application/json')) throw new ApiError(415, 'Content-Type must be application/json');
      if (Date.now() - rateStart > 60000) { rateStart = Date.now(); requests = 0; }
      if (++requests > (config.requestsPerMinute || 60)) throw new ApiError(429, 'Rate limit reached', 'rate_limit');
      if (active >= (config.concurrency || 1)) { res.setHeader('Retry-After', '5'); throw new ApiError(429, 'Ghost is busy; retry shortly', 'busy'); }
      active++; slot = true;
      const body = await readBody(req, config.maxBytes || 786432);
      controller = new AbortController();
      const timeoutMs = config.timeoutMs ?? 180000;
      if (timeoutMs > 0) timer = setTimeout(() => controller.abort(), timeoutMs);
      res.once('close', () => { if (!res.writableFinished) controller.abort(); });
      if (path === '/v1/ghost/classify') {
        if (typeof body.input !== 'string' || !body.input.trim() || body.input.length > 2000) throw new ApiError(400, 'Expected input text, maximum 2000 characters');
        if (!classify) throw new ApiError(503, 'Policy classifier not installed', 'classifier_unavailable');
        const result = await classify(body.input, controller.signal);
        return json(res, 200, result);
      }
      validateRequest(body, config.modelId);
      const profile = req.headers['x-ghost-context-profile'] || 'preserve';
      const prepared = applyContextProfile(body, profile);
      const contentChars = messages => messages.reduce((sum, message) => sum + (typeof message.content === 'string' ? message.content.length : JSON.stringify(message.content || '').length), 0);
      log({ requestId, outcome: 'started', contextProfile: profile, profileApplied: prepared.applied,
        inputChars: contentChars(body.messages), forwardedChars: contentChars(prepared.request.messages),
        messageCount: body.messages.length, toolCount: body.tools?.length || 0, forwardedToolCount: prepared.request.tools?.length || 0, toolRoute: prepared.toolRoute || 'preserve' });
      const evidence = boundEvidence(retrieve(evidenceQuery(latestUserText(body)), documents), prepared.applied ? 1000 : 4000);
      // Only the opted-in interactive client receives service-authored notices.
      // These are not model tokens, proof of progress, or an extended deadline.
      if (body.stream && prepared.applied && !body.response_format) {
        const intervalMs = config.progressIntervalMs ?? 240000;
        if (intervalMs > 0) progressTimer = setInterval(() => {
          if (controller.signal.aborted || res.destroyed || res.writableEnded) return;
          if (!res.headersSent) res.writeHead(200, { 'Content-Type': 'text/event-stream', 'Cache-Control': 'no-cache, no-store', 'X-Accel-Buffering': 'no' });
          progressFrame ??= { id: `chatcmpl-${requestId}`, object: 'chat.completion.chunk', created: Math.floor(started / 1000), model: config.modelId };
          const notice = '[Aviso automático do serviço Ghost] Ainda aguardando a inferência local; a tarefa não está confirmada como concluída.\n\n';
          res.write(`data: ${JSON.stringify({ ...progressFrame, choices: [{ index: 0, delta: { ...(progressCount ? {} : { role: 'assistant' }), content: notice }, finish_reason: null }] })}\n\n`);
          progressCount++;
          log({ requestId, outcome: 'waiting', elapsedMs: Date.now() - started, automaticProgressCount: progressCount });
        }, intervalMs);
      }
      const result = await infer(prepared.request, evidence, controller.signal);
      if (progressTimer) clearInterval(progressTimer);
      const output = completion(result.message, config.modelId, result.usage, result.finishReason);
      if (body.stream) {
        if (progressFrame) { output.id = progressFrame.id; output.created = progressFrame.created; }
        if (!res.headersSent) res.writeHead(200, { 'Content-Type': 'text/event-stream', 'Cache-Control': 'no-cache, no-store', 'X-Accel-Buffering': 'no' });
        for (const chunk of streamChunks(output, body.stream_options?.include_usage)) res.write(`data: ${JSON.stringify(chunk)}\n\n`);
        res.end('data: [DONE]\n\n');
      } else json(res, 200, output);
      log({ requestId, outcome: 'completed', elapsedMs: Date.now() - started, evidenceCount: evidence.length, toolProposals: result.message.tool_calls?.length || 0 });
    } catch (error) {
      const status = controller?.signal.aborted ? 504 : error.status || 502;
      const code = controller?.signal.aborted ? 'timeout' : error.code || 'backend_error';
      const failure = { error: { message: controller?.signal.aborted ? 'Ghost did not finish within its request budget; use a shorter conversation or a smaller task' : error instanceof ApiError ? error.message : 'Ghost backend request failed', type: 'ghost_error', code } };
      if (!res.destroyed && !res.headersSent) json(res, status, failure);
      else if (!res.destroyed && !res.writableEnded) {
        // Never convert a timeout after a notice into a successful completion.
        res.end(`data: ${JSON.stringify(failure)}\n\ndata: [DONE]\n\n`);
      }
      log({ requestId, outcome: 'error', status, code, elapsedMs: Date.now() - started });
    } finally { if (slot) active--; if (timer) clearTimeout(timer); if (progressTimer) clearInterval(progressTimer); }
  });
}

export async function main() {
  const root = fileURLToPath(new URL('../', import.meta.url));
  const config = {
    host: process.env.GHOST_HOST || '127.0.0.1', port: Number(process.env.GHOST_PORT || 18791), modelId: 'ghost-ji',
    backend: process.env.GHOST_BACKEND || 'openai-compatible', upstreamModel: process.env.GHOST_UPSTREAM_MODEL || 'ghost-local-qwen3-4b',
    upstreamUrl: process.env.GHOST_UPSTREAM_URL || 'http://127.0.0.1:18792/v1', upstreamKey: process.env.GHOST_UPSTREAM_KEY_FILE ? (await readFile(process.env.GHOST_UPSTREAM_KEY_FILE, 'utf8')).trim() : undefined,
    fallbackUrl: process.env.GHOST_FALLBACK_URL,
    primaryTimeoutMs: Number(process.env.GHOST_PRIMARY_TIMEOUT_MS || 1200000),
    openclawBin: process.env.GHOST_OPENCLAW_BIN || 'openclaw', agent: process.env.GHOST_OPENCLAW_AGENT || 'coordinator',
    apiKey: (await readFile(process.env.GHOST_API_KEY_FILE, 'utf8')).trim(), timeoutMs: Number(process.env.GHOST_TIMEOUT_MS || 600000), concurrency: 1
  };
  if (!Number.isInteger(config.timeoutMs) || (config.timeoutMs !== 0 && (config.timeoutMs < 1000 || config.timeoutMs > 1800000))) throw new Error('Invalid GHOST_TIMEOUT_MS');
  if (!Number.isInteger(config.primaryTimeoutMs) || (config.primaryTimeoutMs !== 0 && (config.primaryTimeoutMs < 1000 || config.primaryTimeoutMs > 1800000))) throw new Error('Invalid GHOST_PRIMARY_TIMEOUT_MS');
  if (!Number.isInteger(config.port) || config.port < 1 || config.port > 65535) throw new Error('Invalid GHOST_PORT');
  if (!['127.0.0.1', '::1', 'localhost'].includes(config.host) && process.env.GHOST_TLS_PROXY !== 'configured') throw new Error('Non-loopback binding requires an explicitly configured TLS proxy');
  const documents = await loadKnowledge(resolve(root, 'knowledge'));
  const policyBin = process.env.GHOST_POLICY_BIN;
  const classify = policyBin ? async (input, signal) => JSON.parse(await runProcess(policyBin, [], { input, signal, maxBytes: 16384 })) : undefined;
  const server = createGhostServer({ config, documents, infer: createInference(config), classify, log: event => process.stdout.write(`${JSON.stringify(event)}\n`) });
  server.listen(config.port, config.host, () => process.stdout.write(`Ghost API listening on ${config.host}:${config.port}; backend=${config.backend}; model=${config.modelId}\n`));
  for (const sig of ['SIGTERM', 'SIGINT']) process.on(sig, () => { server.close(); setTimeout(() => process.exit(0), 5000).unref(); });
}

if (process.argv[1] && realpathSync(process.argv[1]) === fileURLToPath(import.meta.url)) main().catch(() => { process.stderr.write('Ghost failed to start; check protected configuration and service dependencies.\n'); process.exitCode = 1; });
