# Case 003 — Heart Pass Quota Layer

Status: first local runtime/UI slice implemented; sponsored backend and real usage accounting still pending.

## 1. Interpretation

The Heart Pass is no longer only a public concept or a wallet-verification experiment. The current SolOS implementation can already identify the anchor asset as ERC-1155, check `balanceOf(wallet, tokenId)` on Polygon, and gate Ghost/Brave onboarding behind `verified-holder`.

The next useful step is therefore not another pass explanation. It is the **Quota Layer**: a small, legible usage contract that turns verified ownership into measurable Ghost research utility.

As of 2026-07-08, SolOS now implements the first local slice of that contract: `config/heart_pass.json` carries `quotaLayer`, `runtime-core` emits `heartPass.quotaLayer`, and the Wallet plus Agent/Ghost surfaces render quota state. The status remains `verification-required` until the pass is verified, and no sponsored backend is called yet.

## 2. Product thesis

The Heart Pass should mean:

- this wallet holds the pass;
- this holder can enter the Ghost research flow;
- this month has a visible usage allowance;
- this request uses either sponsored quota or the user's own key;
- the system shows what remains and what happens next.

It should not mean:

- guaranteed profit;
- investment return;
- hidden shared API spending;
- unlimited AI/search usage;
- automatic billing without consent.

## 3. Target architecture

Recommended direction: **hybrid quota with BYOK fallback**.

### Sponsored quota path

Use when:
- the holder is verified;
- the monthly included quota has remaining requests;
- the quota service is reachable;
- the request type is allowed for the current experiment.

Behavior:
- SolOS sends the wallet/pass proof and request metadata to a server-side quota service.
- The quota service calls Brave/OpenAI with server-side credentials.
- The response returns content plus quota metadata.
- The shell shows remaining allowance and reset period.

### BYOK fallback path

Use when:
- the holder is not verified;
- sponsored quota is exhausted;
- the quota service is unavailable;
- the user explicitly prefers their own key.

Behavior:
- SolOS guides the user through Brave key setup.
- The key remains local to the user.
- Ghost shows that the user's own account is paying for the request.

## 4. Runtime contract draft

The native shell receives a quota object inside Heart Pass runtime state:

```json
{
  "heartPass": {
    "status": "verified-holder",
    "walletAddress": "0x...",
    "network": "polygon",
    "contract": "0x507783149b7abb6ce23414dd0c9742eb9f4549b4",
    "tokenId": "1",
    "standard": "ERC-1155",
    "quotaLayer": {
      "status": "planned",
      "mode": "hybrid-sponsored-byok",
      "period": "local-pilot",
      "includedQueries": 25,
      "usedQueries": 0,
      "remainingQueries": 25,
      "fallback": "byok",
      "usageSource": "not-active",
      "lastSync": "never",
      "resetPolicy": "manual until quota service exists"
    }
  }
}
```

Status values should stay explicit:

- `planned`
- `inactive`
- `available`
- `exhausted`
- `service-unavailable`
- `byok-active`
- `verification-required`

## 5. First implementation slice

The next engineering pass should be deliberately small:

1. [x] Add a local `quotaLayer` field to `solos/config/heart_pass.json`.
2. [x] Load it into the runtime snapshot.
3. [x] Render a Heart Pass Quota card in Wallet and Agent/Ghost surfaces.
4. [x] Do not call a backend yet.
5. [x] Use UI copy that explains verification requirement, planned quota, and BYOK fallback.
6. [x] Smoke-check that `heartPass.quotaLayer` appears in the generated runtime snapshot and the native shell builds.

This creates the user-facing contract before infrastructure cost is introduced.

## 6. Second implementation slice

After the local contract is visible:

1. Define a quota service endpoint shape.
2. Add signed wallet/session proof or equivalent authentication.
3. Add monthly wallet/pass quota accounting.
4. Proxy a narrow Ghost research call through Brave Search.
5. Return result metadata with remaining quota.
6. Log usage by wallet, pass, capability, provider, and month.

## 7. Supervision requirements

The firm/operator should be able to see:

- total monthly sponsored pool;
- requests by wallet/pass;
- remaining pool;
- provider errors;
- quota-abuse spikes;
- fallback-to-BYOK events;
- estimated cost per public experiment.

The user should be able to see:

- verified pass state;
- current quota period;
- remaining included searches;
- whether Ghost is using sponsored quota or BYOK;
- next action when quota is exhausted.

## 8. Public wording

Recommended public phrase:

> The next Heart Pass step is a Quota Layer: a transparent usage allowance that lets verified holders understand when Ghost is using sponsored research capacity, when it falls back to the user's own key, and what remains in the current period.

Avoid:

- "earn with the pass"
- "passive income"
- "guaranteed access forever"
- "unlimited AI credits"
- "free credits paid by the NFT"

## 9. Success condition

The next step succeeds when a user can open SolOS, verify the Heart Pass, see a planned or active monthly quota, understand who pays for Ghost research, and continue through BYOK when sponsored capacity is unavailable.

That is the practical bridge from symbolic pass to usable operating-layer benefit.
