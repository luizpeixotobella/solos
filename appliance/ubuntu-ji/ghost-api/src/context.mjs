// Avoid retrieving project doctrine solely because the user addresses Ghost.
// Keep the released lexical retriever separate from experimental candidates.
export function evidenceQuery(text) {
  const query = String(text).replace(/^Runtime: name=[^\n|]+\s*\|\s*agent=[^\n|]+\s*\|\s*model=[^\n|]+\s*\n/, '').trim();
  // Literal-output transport probes need no documentary context. Match the
  // request form, not any particular marker or previously observed example.
  if (/^(?:Reply with exactly|Responda exatamente)\s*:/i.test(query)) return '';
  return query.replace(/\b(?:ghost|solos)\b/gi, ' ').trim();
}

// Bound additional evidence independently of caller-owned conversation history.
// Truncation is explicit and keeps source attribution on every retained item.
export function boundEvidence(evidence, maxChars = 4000) {
  const perDocument = Math.floor(maxChars / Math.max(1, evidence.length));
  return evidence.map(item => ({ ...item, text: item.text.length <= perDocument ? item.text : `${item.text.slice(0, Math.max(0, perDocument - 32))}\n[Source excerpt truncated]` }));
}
