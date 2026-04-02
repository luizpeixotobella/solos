# SolOS Roadmap v0

## Phase 0 — Foundation

Deliverables:

- project name
- README
- architecture v0
- roadmap v0
- product framing

Outcome:

- project can be discussed clearly
- technical direction is constrained enough to build

## Phase 1 — Shell Prototype

Deliverables:

- shell app scaffold
- dashboard UI
- profile/session placeholder
- agent panel UI
- app launcher UI

Outcome:

- first visible SolOS experience exists

## Phase 2 — Agent Integration

Deliverables:

- OpenClaw integration concept finalized
- basic shell-to-agent bridge
- command surface for safe actions
- remote interaction concept via WhatsApp/OpenClaw

Outcome:

- SolOS is not just visual; it becomes interactive and operational

## Phase 3 — Solana Integration

Deliverables:

- wallet connection flow
- balance/account view
- signing UX design
- first on-chain or chain-aware app interaction

Outcome:

- SolOS proves real web3 utility

## Phase 4 — App Runtime

Deliverables:

- app registry format
- capability model draft
- first example app
- launcher opening real apps

Outcome:

- SolOS starts behaving like a platform, not only a dashboard

## Phase 5 — VM Packaging

Deliverables:

- documented setup for running SolOS in a VM
- startup flow into shell environment
- packaged demo instructions

Outcome:

- SolOS can be shown as a working environment, not just source code

## Phase 6 — Product Hardening

Deliverables:

- permission review
- wallet security review
- UX refinement
- branding/design polish
- installation strategy discussion

Outcome:

- project is ready for repeated demos and deeper development

## Immediate next actions

1. Choose shell stack:
   - Tauri + React
   - or Electron + React
2. Define MVP screens.
3. Scaffold repository for the shell prototype.
4. Define how OpenClaw will integrate locally.
5. Design the wallet UX before implementing signing.

## Recommendation

Start with a shell prototype, not a custom distro.
That gives the project something visible, testable, and fundable quickly.
