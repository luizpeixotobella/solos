# SolOS v1.0 RC1 Release Gate

Date: 2026-08-19

## Release definition

SolOS v1.0 RC1 is a demonstrable Linux-based operating layer above a Rust runtime intermediary. It does not claim to be a kernel, a production wallet, a hosted AI provider, or an autonomous general-purpose agent.

## Shipped contracts

- Runtime snapshot schema: `solos.runtime.snapshot.v1`.
- Default-deny capability manifest with local, network, write, wallet, and public scopes.
- Persistent Ghost route evaluation seed with accepted, rejected, and corrected outcome states.
- First mediated action: open the Workspace module only after visible approval.
- Ghost Resolution Loop schema `solos.ghost.resolutions.v1`, with a Daemon-owned durable state store and legal selected-to-resolved transitions.
- Native and web resolution surfaces with target outcome, ordered plan, approval, progress, result and retained evidence.
- Heart Pass ERC-1155 state and a signed-proof requirement for any future sponsored call.
- Provider-neutral quota proxy contract with idempotency and BYOK fallback.
- Scoped memory classes with retention and revocation semantics.
- Web shell and native Qt/QML shell surfaces.
- Browser-kiosk appliance path and live-build ISO scaffold.

## Verification commands

Run from the repository root:

```bash
./tools/verify-v1.sh
```

The gate checks Rust formatting/tests, runtime JSON invariants, Ghost resolution RPC and restart persistence, web-shell build, native-shell build when Qt is available, appliance scripts, and documentation.

## Known non-production boundaries

- Sponsored Brave/OpenAI calls remain disabled until a real backend verifies signed holder sessions.
- Wallet signing is never executed by the demo runtime.
- Pulso remains preview/prototype until consent, moderation, export/delete, ledger, caps, and anti-fraud rules run server-side.
- Native shell app launching is contract-visible but intentionally does not spawn arbitrary commands.
- The first resolution proves one bounded local action. Research and public-send candidates remain unavailable until their real quota and account-bound adapters exist.

## Candidate decision

Tag `v1.0.0-rc1` only after:

- the verification script passes;
- the demo is smoke-tested;
- release notes and public copy match the actual implementation;
- no secrets or generated build artifacts are staged.
