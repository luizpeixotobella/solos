# SolOS engineering operating principles

Status: working doctrine for implementation as SolOS grows.

## Why this exists

SolOS is moving from concept/prototype into a larger system. Implementation should therefore be guided by engineering as a full discipline, not only by coding practice.

The working stance is: **engineer with discipline, design with modular boundaries, test the seams, observe the runtime, and preserve the human/ownership philosophy that makes SolOS meaningful.**

## Current research anchors

- The engineering design process starts with defining the problem, objectives, and constraints, then generating alternative solutions, evaluating/selecting among them, designing, prototyping, testing, evaluating performance, and reporting/iterating.
- Test automation and build pipelines shorten feedback loops from days/weeks to seconds/minutes and make safe change possible. Martin Fowler's practical test pyramid frames a maintainable portfolio around many fast lower-level tests and fewer expensive end-to-end tests.
- Observability should let us ask “why is this happening?” from outside the system. OpenTelemetry frames useful instrumentation around traces, metrics, and logs, plus user-facing reliability indicators.
- Modular-monolith thinking is appropriate for this phase: one coherent repo/build can still have strong internal module boundaries, clear interfaces, and encapsulated responsibilities before any later extraction.
- Architecture Decision Records are useful when decisions become significant: capture context, options, decision, consequences, and status so the project has institutional memory.

## Engineering loop for SolOS

For meaningful changes, use this loop:

1. **Problem framing** — what problem is SolOS solving now, for whom, and under what constraints?
2. **Requirements and non-goals** — what must be true, and what are we explicitly not doing yet?
3. **Option generation** — identify at least 2 plausible approaches when architecture/tradeoff matters.
4. **Tradeoff analysis** — compare complexity, safety, user legibility, cost, reversibility, and philosophical fit.
5. **Small design/prototype** — implement the smallest slice that proves the seam.
6. **Verification** — run the cheapest meaningful gate: format/check/build/smoke/test/inspection.
7. **Supervision** — expose state/status/errors so the system can be understood from outside.
8. **Documentation/memory** — document architectural decisions and update project memory when direction changes.
9. **Iteration** — use results to refine the next slice.

## Operating rules for SolOS work

1. **Small slices over theatrical rewrites**
   - Prefer one visible, reversible capability at a time.
   - Keep each slice tied to a real runtime or UI boundary.

2. **Modular boundaries first**
   - Treat Rust runtime-core, C++/Qt bridge/controller, QML surfaces, config files, docs, and future backend/wallet services as separate layers.
   - Avoid leaking provider details or wallet/API secrets into UI-only layers.

3. **Typed contracts over scattered literals**
   - New capabilities should enter through explicit structs/schema/config where possible.
   - Runtime snapshot additions should be named, stable, and readable.

4. **Test and verification gates**
   - For Rust: run `cargo fmt`, `cargo check`, and targeted `cargo run`/JSON inspection.
   - For shell-native: run `cmake --build .`; when practical, perform a short smoke launch and inspect logs.
   - For CMS/Next: run build; lint failures should be separated into new vs pre-existing issues.

5. **Observability and supervision**
   - Every feature should expose enough status for a user/operator to know what is configured, pending, failed, or verified.
   - Prefer explicit statuses such as `needs-wallet`, `wallet-configured-unverified`, `verified-holder`, `not-holder`, `verification-error`.

6. **Security and ownership honesty**
   - Do not hide shared developer keys in public client surfaces.
   - Keep BYOK and billing boundaries legible.
   - Wallet actions must stay explicit and approval-oriented.

7. **Documentation as part of implementation**
   - Update docs when a feature changes architecture, product promise, or user-facing status.
   - Keep public wording away from investment/yield promises for Heart Pass.

8. **Philosophical coherence**
   - SolOS should not merely add features; it should make agency, limits, permissions, cost, identity, and control legible.
   - Engineering decisions should support that thesis rather than obscure it.

## Near-term application to Heart Pass

Heart Pass should progress as staged slices:

1. visible runtime/UI pass surface;
2. local config and wallet capture;
3. Polygon ownership verification;
4. Ghost/Brave onboarding gated by verified holder state;
5. local usage/quota metering;
6. optional hosted quota/proxy layer only when billing and supervision are explicit.
