# SolOS Founder Rewards contract

## Purpose

Founder Rewards connect verified campaign support in the LBArtes CMS to visible, bounded utility inside SolOS. They are not financial instruments.

## Cross-system flow

1. CMS validates a contribution and grants `founder-heart` plus Pulso Credits.
2. Supabase stores the supporter profile, account, ledger and redemption.
3. The user exchanges 10 credits for 25 Ghost queries.
4. The authenticated rewards page exposes a high-entropy claim code.
5. Native Wallet requires a locally verified Heart Pass before submitting the claim.
6. The CMS binds the redemption atomically to the first Polygon wallet.
7. Wallet persists the redemption ID and increases the visible Ghost allowance once.
8. Rust quota accounting decrements explicitly through `consume-ghost-query`; snapshot refresh never spends quota.

## Trust boundaries

- CMS/Supabase is the campaign and ledger authority.
- Wallet owns local holder verification and entitlement synchronization.
- Rust runtime owns local quota accounting.
- Provider-backed sponsored execution remains disabled until a signed-proof proxy exists.
- BYOK remains the fallback and keeps provider billing under user control.

## Idempotency

- campaign grant: unique contribution reference;
- redemption: atomic database transaction;
- wallet sync: persisted redemption ID;
- quota consumption: explicit state transition, never UI refresh.

## Failure states

- invalid/unavailable claim;
- claim bound to another Wallet;
- Heart Pass not verified;
- reward already synchronized;
- quota inactive or exhausted;
- CMS unavailable;
- provider proxy unavailable, with BYOK fallback.

## Public non-promises

Founder Rewards do not represent equity, debt, dividends, yield, cash value, guaranteed access forever, guaranteed provider availability or an investment return. Pulso Credits are internal utility credits.

## Operational status — 2026-08-02

The database grant, redemption, atomic claim, Wallet synchronization contract and Rust local meter are implemented and tested. Public deployment and Stripe configuration remain separate release steps.
