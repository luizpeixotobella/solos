# SolOS

SolOS is an operating environment built around agency, explicit ownership, and ambient computation.

## Current focus

- Native shell prototype in Qt/QML
- Clear information architecture for Home, Agent, Wallet, and Apps
- Converting web-shell semantics into native system surfaces
- Building the conceptual thesis alongside the implementation
- Developing Ghost from the AI-era inversion: **data + results = algorithms**
- Making the Heart Pass Quota Layer visible as a local runtime contract before adding sponsored backend calls
- Promoting Ghost from broad intelligence doctrine into visible request classification, action trace, and route explanation
- Planning SolOS Pulso as a future social app surface with consented human signals and Pulso Credits as internal utility credits
- Tracking SolOS to v1.0 through a living completion plan with done, partial, next, prototype, and release milestones

## Ghost intelligence doctrine

Classical programming is usually framed as:

```text
algorithms + data = results
```

Ghost is being developed from the inverse AI-era synthesis:

```text
data + results = algorithms
```

The point is not that algorithms vanish. The point is that, in learning systems, useful data plus observed or desired results can shape the algorithmic behavior itself. Ghost should therefore collect local/runtime facts, grounded research, cached knowledge, and approval outcomes, then synthesize safer routing and action logic inside SolOS.

## Repository direction

This repository should evolve in two layers:

1. **Thesis / doctrine** — why this OS should exist, what it believes about software, agency, coordination, and ownership
2. **Implementation** — the native shell, app model, wallet surfaces, approval UX, and system integrations

## Active native shell

Path:

- `solos/app/shell-native`

Current stack:

- Qt 6 / QML
- C++ controller bridge
- model-backed shell surfaces replacing ad hoc UI strings

Current implementation direction:

- `HomeState` now gives Home a structured summary and a legible next-action concept
- Home is moving away from generic placeholder copy toward explicit environment guidance
- the native shell is gradually inheriting semantics from the earlier web-shell prototype while becoming a real native system surface

## Documentation map

- `docs/thesis.md` — philosophical and product thesis behind SolOS
- `docs/architecture.md` — system architecture and component map
- `docs/ghost-intelligence-doctrine.md` — Ghost doctrine for the AI-era inversion from `algorithms + data = results` to `data + results = algorithms`
- `docs/ghost-operational-readiness-2026.md` — readiness model for Ghost as an operating agent: grounding, memory, tools, approvals, traces, and language mediation
- `docs/cases/003-heart-pass-quota-layer.md` — local quota contract and next backend path for Heart Pass holder utility
- `docs/cases/004-ai-ghost-classification-trace.md` — first W3Schools-inspired Ghost classifier, action trace, and route explanation slice
- `docs/solos-pulso-credits.md` — planned SolOS Pulso social surface and Pulso Credits utility-credit model
- `docs/ai-ghost-w3schools-ai-archive-2026-07-08.md` — archived W3Schools AI/Machine Learning topics translated into the Ghost backlog
- `docs/product-completion-plan.md` — live engineering plan for finishing SolOS v1.0, with current status, milestones, prototype board, and discussion decisions
- `docs/architecture-v1.md` — SolOS v1.0 architecture with runtime as intermediary between Linux and SolOS
- `docs/solos-v1.0.md` — definition of the SolOS 1.0 release posture
- `docs/roadmap.md` — implementation roadmap
- `docs/devlog.md` — chronological development notes

## Current doctrine for technical migration

SolOS should adopt Rust selectively where it improves safety, correctness, or long-term architecture.

That does **not** imply a blanket rewrite.

Preferred rule:

- new security-sensitive or concurrency-heavy subsystems may default toward Rust
- stable prototype/UI bridge code may remain in C++ until there is a concrete migration reason
- migration should happen by subsystem boundary, not by ideology
- Linux remains the base system for SolOS v1.0
- the runtime is an intermediary layer between Linux and the SolOS operating layer

The first active boundary is now defined as a **runtime intermediary seam**:

- `app/runtime-core` begins a Rust subsystem that reads Linux host state and mediates it into stable SolOS-facing runtime contracts
- `app/shell-native` remains the native Qt/QML presentation layer
- the shell consumes structured runtime-mediated orchestration state without collapsing into a rewrite
- SolOS stays an operating layer above the runtime instead of pretending Rust replaced Linux or pretending runtime is just another name for Linux

This gives SolOS a practical migration path instead of an ideological one.

## Ghost readiness

Ghost now exposes operational readiness, not only presence. The native shell can show whether Ghost is merely foundational or ready to operate across grounded research, long-term memory, tool/MCP boundaries, human approvals, observability, and language mediation.

This is the current product rule: Ghost can speak before it acts, research before it executes, and suggest before it writes, signs, pays, posts, or changes host state.

Ghost now also exposes its first deterministic request classifier and action trace. Before a route is treated as executable, the shell can show the request class, safety level, required tools, approval needs, quota cost, selected route, and plain-language route explanation.

## Working principle

Document while building. The idea should mature with the code, not after it.

## Documentation discipline

SolOS should be documented as a live system across two synchronized surfaces:

1. **Product repository** — thesis, architecture, roadmap, devlog, and implementation notes
2. **Institutional CMS** — executive framing, launch collateral, public narrative, and referenced campaign material

When the thesis sharpens or implementation meaningfully advances, both surfaces should be reviewed so the internal doctrine and external framing do not drift apart.
