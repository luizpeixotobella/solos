# SolOS Heart Pass — Brave credits and utility NFT plan

## Status

Draft feature concept for the initial SolOS Pass. This is product/utility framing only, not a financial-return promise.

## Source asset

Initial anchor NFT:

- **Name:** Anastacia Our Hearts #1
- **Collection:** Anastacia Gen Art by LBArtes Luiz
- **Network:** Polygon
- **Contract:** `0x507783149b7abb6ce23414dd0c9742eb9f4549b4`
- **Token standard:** ERC-1155
- **Token ID:** `1`
- **OpenSea:** <https://opensea.io/item/polygon/0x507783149b7abb6ce23414dd0c9742eb9f4549b4/1>

This asset can represent the first symbolic pass while the project validates the utility model.

## Product promise

The pass should be presented as a utility/membership artifact for SolOS:

- early-supporter identity
- access to experimental SolOS/Ghost capabilities
- guided setup for user-owned API keys
- optional monthly search/AI allowance where technically and commercially possible
- transparent usage limits and no promise of investment return

Recommended public wording:

> The SolOS Heart Pass is a symbolic utility pass for early supporters. It can unlock guided onboarding, experimental Ghost features, and usage allowances when available. It is not a promise of profit, yield, revenue share, or guaranteed resale value.

## Brave Search starting point

Brave Search API pricing currently advertises:

- Search: **US$5 per 1,000 requests**
- Free **US$5 in credits every month**, automatically applied to the account
- Capacity around **50 requests/second** for Search

Source checked: Brave Search API pricing page, 2026-05-06.

This means the first SolOS implementation should assume a **bring-your-own Brave key** model by default. SolOS can guide each user to create a Brave API account/key and then validate/save the key locally, which already aligns with the existing Ghost onboarding direction.

## Per-user credits: preferred model

Preferred initial flow:

1. User owns or receives the SolOS Heart Pass.
2. SolOS recognizes the pass as an eligibility signal.
3. User is guided to create/configure their own Brave API key.
4. SolOS stores that key locally in the user's own SolOS config.
5. Ghost tracks local monthly usage and warns before the user's Brave allowance is exceeded.

Why this is preferable:

- keeps billing boundaries legible
- avoids shipping a shared developer key
- lets Brave's own free monthly credit attach to the user's account
- avoids SolOS silently spending from the creator's account
- scales better for early public demos

## Sponsored-credit model

A later model may sponsor user usage from a SolOS treasury/account, but it should be treated as a separate billing layer:

- SolOS account creates a central Brave/OpenAI billing pool
- each SolOS user has an internal monthly quota
- backend proxy enforces user-level quotas before calling external APIs
- every call is attributable to a user, pass, capability, and month
- admin dashboard shows remaining pool and per-user spend

This requires a hosted backend/proxy and should not be done by putting the creator's API keys in public clients.

## MetaMask / wallet automation

MetaMask itself is a wallet, not a general-purpose automated payment API suitable for silently topping up SaaS credits.

Recommended posture:

- use wallet signatures for authentication/proof-of-ownership
- use on-chain payments for buying the pass or topping up a SolOS credit balance
- keep SaaS billing through normal providers or a backend treasury service
- do not ask MetaMask to auto-pay Brave/OpenAI directly from a browser/client without explicit user approval

Possible later architecture:

1. User connects wallet.
2. SolOS verifies the NFT/pass.
3. User optionally pays/tops up an on-chain or off-chain SolOS credit balance.
4. A backend converts that budget into Brave/OpenAI usage through provider billing accounts.
5. SolOS enforces quotas and shows transparent usage.

## Initial feature slice for public launch

Minimum LinkedIn/X/Instagram-worthy slice:

- SolOS page explains the Heart Pass concept.
- Product/CMS page links the OpenSea NFT.
- Ghost onboarding keeps the user-owned Brave API key model.
- Documentation states the next step: local monthly usage metering and pass-gated guided onboarding.

## Next step: Quota Layer

The next product step is the **Heart Pass Quota Layer**.

The pass already has a clearer technical path: local wallet capture, Polygon ERC-1155 verification, and Ghost/Brave onboarding gated by `verified-holder`. The next layer should make the pass useful in a measurable way by showing a monthly usage allowance for Ghost research.

Recommended interpretation:

- Heart Pass verification proves eligibility.
- A quota layer explains the current monthly allowance.
- Ghost can use sponsored capacity only through a server-side quota service.
- BYOK remains the fallback when sponsored quota is unavailable, exhausted, or not yet implemented.
- The UI must always show whether usage is sponsored or user-owned.

This is documented as Case 003 in `docs/cases/003-heart-pass-quota-layer.md`.

## Native implementation status

Current SolOS native implementation:

- `config/heart_pass.json` stores local pass state, wallet address, token metadata, last check, and verification status.
- `config/heart_pass.json` now also stores a local `quotaLayer` contract with planned hybrid sponsored/BYOK mode, included/used/remaining query counts, fallback, usage source, last sync, and reset policy.
- `runtime-core` includes the Heart Pass in the runtime snapshot.
- `runtime-core` includes `heartPass.quotaLayer` in the runtime snapshot, disabled as `verification-required` until the pass is verified.
- Wallet Hub exposes local wallet capture and explicit Polygon verification.
- Wallet Hub and Agent/Ghost both render a Heart Pass Quota card so the allowance and fallback model are visible before a sponsored backend exists.
- Because the anchor asset is ERC-1155, verification uses `balanceOf(wallet, tokenId)` through Polygon JSON-RPC and maps results to `verified-holder`, `not-holder`, or `verification-error`.
- Ghost/Brave onboarding is gated by `verified-holder`: the UI disables Brave key entry until Heart Pass verification succeeds, and the controller refuses Brave key save/validation unless the pass is verified.
- Sponsored provider calls are not implemented yet. The next technical layer is signed holder/session proof plus a server-side quota/proxy service.

Success condition:

> A user can understand that the NFT is a SolOS utility pass, see the Brave/API ownership model, and understand that any credits are usage benefits rather than investment yield.
