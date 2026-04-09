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
- [ ] Wallet overview model
- [~] App registry surface with richer metadata
- [~] Approval queue UI

## Phase 2 — Real system integration

- [ ] Connect shell state to real runtime data
- [ ] Connect approvals to real command/task state
- [ ] Connect wallet panel to real account state
- [ ] Add launcher bridge for modules/apps

## Phase 3 — OS identity

- [ ] Formalize visual language
- [ ] Define interaction grammar for approvals and agent actions
- [ ] Write expanded thesis / manifesto
- [ ] Turn doctrine into product constraints

## Phase 4 — Selective systems migration

- [ ] Define which subsystems should stay in C++ for now
- [ ] Identify Rust candidates by safety/concurrency/integration risk
- [ ] Prototype one Rust subsystem behind a clear interface boundary
- [ ] Establish C++/Rust interoperability rules for the shell and future runtime layers

## Migration principle

Rust should enter SolOS gradually and intentionally.

Priority targets are the parts most likely to benefit from stronger memory safety and clearer concurrency boundaries, such as:

- runtime/integration services
- approval/task orchestration backends
- parsers and capability layers
- future system-facing modules

The current Qt/QML shell can continue evolving in C++ while those boundaries become clearer.
