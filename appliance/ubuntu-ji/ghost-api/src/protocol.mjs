import { randomUUID } from 'node:crypto';

export class ApiError extends Error {
  constructor(status, message, code = 'invalid_request') { super(message); this.status = status; this.code = code; }
}

export function validateRequest(body, modelId) {
  if (!body || typeof body !== 'object' || Array.isArray(body)) throw new ApiError(400, 'Expected JSON object');
  if (body.model !== modelId) throw new ApiError(404, 'Unknown model', 'model_not_found');
  if (!Array.isArray(body.messages) || !body.messages.length || body.messages.length > 256) throw new ApiError(400, 'Expected 1–256 messages');
  const roles = new Set(['system', 'developer', 'user', 'assistant', 'tool']);
  for (const m of body.messages) {
    if (!m || !roles.has(m.role)) throw new ApiError(400, 'Invalid message role');
    if (m.content !== null && typeof m.content !== 'string' && !Array.isArray(m.content)) throw new ApiError(400, 'Invalid message content');
    if (Array.isArray(m.content) && m.content.some(p => p?.type !== 'text' || typeof p.text !== 'string')) throw new ApiError(400, 'This model supports text only');
    if (m.role === 'tool' && typeof m.tool_call_id !== 'string') throw new ApiError(400, 'Tool result requires tool_call_id');
  }
  if (body.stream !== undefined && typeof body.stream !== 'boolean') throw new ApiError(400, 'stream must be boolean');
  if (body.n !== undefined && body.n !== 1) throw new ApiError(400, 'Only n=1 is supported');
  for (const k of ['max_tokens', 'max_completion_tokens']) if (body[k] !== undefined && (!Number.isInteger(body[k]) || body[k] < 1 || body[k] > 8192)) throw new ApiError(400, `${k} must be between 1 and 8192`);
  if (body.tools !== undefined && (!Array.isArray(body.tools) || body.tools.length > 128)) throw new ApiError(400, 'Invalid tools');
  const names = new Set();
  for (const t of body.tools || []) {
    if (t?.type !== 'function' || !/^[a-zA-Z0-9_-]{1,64}$/.test(t.function?.name || '') || names.has(t.function.name)) throw new ApiError(400, 'Invalid or duplicate tool');
    names.add(t.function.name);
  }
  const choice = body.tool_choice;
  if (choice !== undefined && !['auto', 'none', 'required'].includes(choice) && !(choice?.type === 'function' && names.has(choice.function?.name))) throw new ApiError(400, 'Invalid tool_choice');
  if ((choice === 'required' || typeof choice === 'object') && !names.size) throw new ApiError(400, 'tool_choice requires tools');
  return body;
}

export function parseEnvelope(text, request) {
  let parsed;
  try { parsed = JSON.parse(text.trim().replace(/^```(?:json)?\s*\n?/, '').replace(/\n?```$/, '')); }
  catch { throw new ApiError(502, 'Backend returned an invalid structured answer', 'backend_protocol_error'); }
  if (!parsed || !Object.hasOwn(parsed, 'content') || !(parsed.content === null || typeof parsed.content === 'string') || !Array.isArray(parsed.tool_calls)) throw new ApiError(502, 'Backend returned an invalid answer shape', 'backend_protocol_error');
  const permitted = new Set((request.tools || []).map(t => t.function.name));
  const toolCalls = parsed.tool_calls.map(call => {
    if (!call || !permitted.has(call.name) || !call.arguments || typeof call.arguments !== 'object' || Array.isArray(call.arguments) || request.tool_choice === 'none') throw new ApiError(502, 'Backend proposed a disallowed tool', 'backend_protocol_error');
    if (typeof request.tool_choice === 'object' && call.name !== request.tool_choice.function.name) throw new ApiError(502, 'Backend ignored named tool choice', 'backend_protocol_error');
    return { id: `call_${randomUUID().replaceAll('-', '')}`, type: 'function', function: { name: call.name, arguments: JSON.stringify(call.arguments) } };
  });
  if ((request.tool_choice === 'required' || typeof request.tool_choice === 'object') && !toolCalls.length) throw new ApiError(502, 'Backend did not propose the required tool', 'backend_protocol_error');
  if (!toolCalls.length && !parsed.content) throw new ApiError(502, 'Backend returned an empty answer', 'backend_protocol_error');
  return { role: 'assistant', content: parsed.content, ...(toolCalls.length ? { tool_calls: toolCalls } : {}) };
}

export function completion(message, model, usage, finishReason) {
  return { id: `chatcmpl-${randomUUID()}`, object: 'chat.completion', created: Math.floor(Date.now() / 1000), model,
    choices: [{ index: 0, message, finish_reason: message.tool_calls?.length ? 'tool_calls' : (['stop', 'length', 'content_filter'].includes(finishReason) ? finishReason : 'stop') }], ...(usage ? { usage } : {}) };
}

export function streamChunks(result, includeUsage = false) {
  const base = { id: result.id, object: 'chat.completion.chunk', created: result.created, model: result.model };
  const msg = result.choices[0].message;
  const chunks = [{ ...base, choices: [{ index: 0, delta: { role: 'assistant', content: '' }, finish_reason: null }] }];
  if (msg.content) chunks.push({ ...base, choices: [{ index: 0, delta: { content: msg.content }, finish_reason: null }] });
  if (msg.tool_calls) chunks.push({ ...base, choices: [{ index: 0, delta: { tool_calls: msg.tool_calls.map((call, index) => ({ index, ...call })) }, finish_reason: null }] });
  chunks.push({ ...base, choices: [{ index: 0, delta: {}, finish_reason: result.choices[0].finish_reason }] });
  if (includeUsage && result.usage) chunks.push({ ...base, choices: [], usage: result.usage });
  return chunks;
}
