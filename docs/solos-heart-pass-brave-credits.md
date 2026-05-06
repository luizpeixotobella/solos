# SolOS Heart Pass — Brave credits and utility NFT plan

## Status

Draft feature concept for the initial SolOS Pass. This is product/utility framing only, not a financial-return promise.

## Source asset

Initial anchor NFT:

- **Name:** Anastacia Our Hearts #1
- **Collection:** Anastacia Gen Art by LBArtes Luiz
- **Network:** Polygon
- **Contract:** `0x507783149b7abb6ce23414dd0c9742eb9f4549b4`
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

Success condition:

> A user can understand that the NFT is a SolOS utility pass, see the Brave/API ownership model, and understand that any credits are usage benefits rather than investment yield.
