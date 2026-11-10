export async function chat({ baseUrl = 'http://127.0.0.1:18791/v1', apiKey, messages, signal }) {
  const url = new URL(baseUrl);
  if (url.protocol !== 'https:' && !(url.protocol === 'http:' && ['127.0.0.1', 'localhost', '[::1]'].includes(url.hostname))) throw new Error('Remote Ghost endpoints require HTTPS; use an SSH tunnel for private HTTP');
  if (url.username || url.password || url.search || url.hash) throw new Error('Invalid Ghost URL');
  const response = await fetch(`${baseUrl.replace(/\/$/, '')}/chat/completions`, { method: 'POST', redirect: 'error', signal,
    headers: { 'Content-Type': 'application/json', Authorization: `Bearer ${apiKey}` }, body: JSON.stringify({ model: 'ghost-ji', messages, stream: false }) });
  if (!response.ok) throw new Error(`Ghost API HTTP ${response.status}`);
  const body = await response.json();
  const text = body.choices?.[0]?.message?.content;
  if (typeof text !== 'string') throw new Error('Ghost returned no text answer');
  return text;
}
