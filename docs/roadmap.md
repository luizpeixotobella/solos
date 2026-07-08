# SolOS Roadmap

## Phase 0 — Foundation

- [x] Boot a Qt/QML native shell
- [x] Add sidebar navigation
- [x] Add top status bar
- [x] Add basic section cards
- [x] Fix first-pass layout issues in cards
- [x] Split `Main.qml` into screen components
- [ ] Introduce reusable shell theme tokens
- [ ] Add scroll support for content-heavy screens

## Phase 1 — Native information architecture

- [~] Home quick actions model
- [~] Home summary / next-action state via `HomeState`
- [~] Agent surface with task/activity feed
- [~] Wallet overview model
- [~] App registry surface with richer metadata
- [~] Approval queue UI

## Phase 2 — Real system integration

- [~] Replace predetermined runtime snapshot content with progressively real Linux-host-derived state
- [~] Connect shell state to real host runtime data
- [~] Model approvals as first-class runtime-backed entities before binding them to real command/task state
- [~] Add Ghost web onboarding with repository-local Brave key configuration and validate-before-save policy
- [~] Add local monthly Brave usage metering so each user can see remaining search allowance
- [~] Add Ghost request classification, action trace, and route explanation to the runtime/UI contract
- [x] Gate the guided Brave/Ghost onboarding with an optional SolOS Heart Pass ownership check
- [ ] Design a backend quota/proxy layer for future sponsored Brave/OpenAI credits without exposing shared provider keys
- [ ] Add callback/deep-link return flow from Brave key onboarding into SolOS
- [ ] Connect approvals to real command/task state
- [ ] Connect wallet panel to real account state
- [ ] Add launcher bridge for modules/apps
- [ ] Introduce clearer capability boundaries so controllers stop behaving like staging points for semi-static copy

## Phase 3 — OS identity

- [ ] Formalize visual language
- [ ] Define interaction grammar for approvals and agent actions
- [ ] Write expanded thesis / manifesto
- [ ] Turn doctrine into product constraints

## Phase 4 — Selective systems migration

- [x] Define which subsystems should stay in C++ for now
- [x] Identify Rust candidates by safety/concurrency/integration risk
- [x] Prototype one Rust subsystem behind a clear interface boundary
- [~] Establish C++/Rust interoperability rules for the shell and future runtime layers

Current first boundary:

- Linux remains the real runtime substrate
- C++/Qt/QML shell remains the presentation layer
- Rust runtime core reads and emits structured host-backed orchestration state
- JSON snapshot contract acts as the first integration seam before a heavier IPC or FFI decision
- the current snapshot now carries live host facts plus a first `systemStatus` contract and richer approval objects
- next iteration should turn this from a structured host snapshot seam into progressively real mediated runtime services and approval execution boundaries
- Ghost now has a first functional prototype path: layered runtime data + optional Brave-grounded research + user-owned key onboarding inside the SolOS repo
- Heart Pass now exposes a local `quotaLayer` contract beside verified ownership state, making planned allowance, usage, fallback, and provider-cost boundaries visible before any sponsored backend exists

## Migration principle

Rust should enter SolOS gradually and intentionally.

Priority targets are the parts most likely to benefit from stronger memory safety and clearer concurrency boundaries, such as:

- runtime/integration adapters over Linux host services
- approval/task orchestration backends
- parsers and capability layers
- future system-facing modules

The current Qt/QML shell can continue evolving in C++ while those boundaries become clearer, and without claiming ownership of the Linux runtime itself.

## Ghost multilingual capability update

The Ghost roadmap now includes multilingual human fluency as a core capability, adjacent to local runtime facts, web-grounded research, approvals, and knowledge caching.

- [~] Represent language support in the runtime snapshot and Agent/Ghost surface.
- [ ] Add language detection for user requests and runtime events.
- [ ] Prefer the user's active language for Ghost replies, approval explanations, onboarding, and task summaries.
- [ ] Preserve source-language citations and translated summaries when Brave/data-mined evidence is used.
- [ ] Treat cultural context, idiom, tone, and register as part of Ghost's intent layer instead of generic translation cleanup.

This update is intentionally modest in implementation but important in direction: Ghost should grow into a multilingual operating agent, not a monolingual assistant with translated UI strings.

## Ghost operational readiness update

Ghost now has an explicit readiness layer in the runtime snapshot and native shell.

- [x] Add `ghost.operationalReadiness` to the runtime-core snapshot.
- [x] Surface readiness status and pillars in the native Agent/Ghost screen.
- [x] Document the 2026 agent-readiness model in `docs/ghost-operational-readiness-2026.md`.
- [ ] Add a typed tool manifest with read/write/sensitive scopes.
- [~] Represent trace summaries for request classification, route selection, quota impact, approval policy, and outcome.
- [ ] Persist trace summaries for retrieval, tool calls, approvals, and outcomes.
- [ ] Split Ghost memory into scoped classes with revocation semantics.
- [ ] Add provider-neutral retrieval adapters beyond Brave.
- [ ] Connect readiness results to the future task/action router.

## Heart Pass Quota Layer update

The first local quota slice is now implemented.

- [x] Add `quotaLayer` to `config/heart_pass.json`.
- [x] Emit `heartPass.quotaLayer` from `runtime-core`.
- [x] Parse quota fields through the Qt runtime bridge and `AppController`.
- [x] Render quota cards in Wallet and Agent/Ghost.
- [x] Keep sponsored quota disabled until Heart Pass verification succeeds.
- [ ] Add real usage decrementing when Ghost research executes.
- [ ] Define the server-side quota/proxy endpoint.
- [ ] Add signed holder/session proof before sponsored provider calls.
