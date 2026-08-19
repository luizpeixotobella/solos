# Ghost Resolution Loop v1

Date: 2026-08-19

Status: RC1 beta slice implemented and verified for one bounded local objective.

## What changed

Ghost now has a resolution as a first-class product object instead of only a request, answer or loose task. A resolution keeps one selected objective linked from intent through a target outcome:

1. select a ready objective;
2. build a bounded plan;
3. stop at a visible approval boundary;
4. execute one declared capability;
5. verify the target state;
6. retain the result and evidence as a resolved record.

The first executable resolution is `resolution-safe-workspace`. It uses `app.open.safe` and verifies the resulting Workspace state with `app.state.read`. The research and public-launch candidates are intentionally visible but unavailable until their quota/BYOK and account-bound publish capabilities exist.

## Natural ownership

- `runtime-core/src/ghost_resolution.rs` owns the Ghost resolution schema and legal transitions.
- `solos-daemon` owns the durable store, atomic writes and RPC transition boundary.
- the aggregate RC1 snapshot remains a compatibility read model and receives the latest Daemon-owned resolution state.
- the native Qt/QML shell and the web shell render the journey and request transitions; they do not own durable state.

This keeps SolOS cohesive outside and modular inside.

## Schema

The store schema is `solos.ghost.resolutions.v1`. Each resolution carries:

- objective and target outcome;
- readiness, status, progress and current step;
- required capability and approval policy;
- ordered steps with per-step results;
- evidence items for selection, planning, approval, execution and verification;
- timestamps and a final result summary.

Current states are `candidate`, `selected`, `awaiting-approval`, `resolved` and `blocked`. Unsupported or out-of-order transitions fail closed.

## Daemon RPC

The owner-only Unix socket exposes these resolution methods:

- `ghost.resolutions.get`
- `ghost.resolution.select`
- `ghost.resolution.start`
- `ghost.resolution.decide`
- `ghost.resolutions.reset`

Example:

```json
{"id":"start-1","method":"ghost.resolution.start","params":{"id":"resolution-safe-workspace"}}
```

Approval is explicit:

```json
{"id":"approve-1","method":"ghost.resolution.decide","params":{"id":"resolution-safe-workspace","approved":true}}
```

The Daemon persists to `SOLOS_GHOST_RESOLUTION_STORE` when configured, then `XDG_STATE_HOME/solos/ghost-resolutions.json`, or the user's local state directory. Writes are committed atomically.

## Honest beta boundary

The v1 loop proves one end-to-end, approval-bound local action. It does not claim autonomous general task execution, real web research quota accounting, arbitrary host commands, wallet actions or public posting. Those candidates remain unavailable until their capabilities, proofs and adapters are implemented.

## Verification

Run:

```bash
tools/verify-v1.sh
```

The verification gate covers Rust transition tests, Daemon RPCs, denial safety, atomic persistence, a Daemon restart, snapshot injection, web build, native build and an offscreen native-shell smoke.
