# Ghost Quota Proxy Contract v1

`POST /v1/ghost/research`

Required headers:

- `Authorization: SolOSSession <signed-holder-session>`
- `Idempotency-Key: <unique-request-key>`
- `Content-Type: application/json`

Request:

```json
{
  "query": "string",
  "providerPreference": "auto",
  "maxResults": 5,
  "traceId": "string"
}
```

Rules:

- Verify session signature, Heart Pass eligibility, expiry, and revocation before provider use.
- Reserve quota atomically and reject duplicate idempotency keys.
- Use a provider-neutral adapter; Brave is the first adapter, not the contract.
- Return citations, charged units, remaining units, provider, and trace identifier.
- Roll back reservation on provider failure.
- Never accept wallet private keys or provider secrets from the shell.
- Return `quota_exhausted`, `proof_required`, `service_unavailable`, and `byok_required` as explicit states.

This repository ships the contract and disabled runtime state. A production service is deliberately not implied.

Founder fundraiser claims may increase a Wallet's visible local allowance, but they do not bypass this proxy contract. Any future provider-backed use must still supply signed holder/session proof, an idempotency key and atomic quota reservation. The fundraiser never places Stripe credentials or provider keys in SolOS.
