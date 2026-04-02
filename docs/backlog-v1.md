# SolOS Backlog v1

## Purpose

This document turns the SolOS vision into a buildable execution map.

The core discipline is simple:

- protect the product thesis
- keep the MVP small enough to ship
- separate real near-term work from distant vision

## Product definition

SolOS v1 is a Linux-based operating layer with:

- a custom shell
- a native agent experience
- wallet and identity surfaces
- an apps launcher/runtime surface
- a coherent demo flow

## MVP boundaries

### In scope
- shell base
- Home screen
- Agent screen
- Wallet screen
- Apps screen
- state management with realistic mocked data
- coherent visual system
- first demo narrative

### Out of scope for MVP
- custom kernel
- full custom distro
- complex on-chain architecture
- tokenomics
- autonomous wallet actions without approvals
- app SDK/platform economy

## Epics

### Epic 0 — Foundation
**Goal:** Establish stable direction and constraints.

Tasks:
- finalize README, MVP, roadmap, architecture, backlog
- define stack choice for shell prototype
- define design principles and system constraints
- define agent safety boundaries

### Epic 1 — Shell Base
**Goal:** Create the visible SolOS environment.

Tasks:
- app shell scaffold
- navigation structure
- top status bar
- panel/card system
- route layout
- theme tokens

### Epic 2 — Home
**Goal:** Answer what matters now.

Tasks:
- identity/welcome area
- system overview cards
- quick actions
- wallet snapshot
- recent activity
- agent pulse

### Epic 3 — Agent
**Goal:** Make the agent feel native to the system.

Tasks:
- conversation view
- suggestions/actions
- active tasks list
- approval flow states
- result cards

### Epic 4 — Wallet / Identity
**Goal:** Surface ownership and signing clearly.

Tasks:
- wallet connection UI
- account summary
- balances and assets
- recent activity
- signature request UI

### Epic 5 — Apps
**Goal:** Show SolOS as a platform layer.

Tasks:
- app grid/list
- search and filters
- app details drawer
- capability badges
- open/launch flow

### Epic 6 — State + System Model
**Goal:** Keep the product from feeling fake.

Tasks:
- global state store
- typed system objects
- mock services
- notifications and approvals state
- seeded demo data

### Epic 7 — Agent Bridge
**Goal:** Prepare the interface between agent and system capabilities.

Tasks:
- define bridge contract
- define safe actions
- define approval-required actions
- map initial commands

### Epic 8 — Branding / Visual System
**Goal:** Give SolOS a recognizable product language.

Tasks:
- color system
- typography
- spacing
- panel treatment
- status indicators
- motion principles

### Epic 9 — Demo Flow
**Goal:** Build a memorable first demonstration.

Tasks:
- define demo script
- seed representative data
- polish transitions and states
- connect Home → Agent → Wallet → Apps story

### Epic 10 — Packaging / Environment
**Goal:** Prepare later movement toward app packaging and VM testing.

Tasks:
- wrapper strategy
- local run strategy
- VM packaging notes
- environment assumptions

## Suggested build order

### Sprint 1
- choose stack
- scaffold shell
- set up theme and layout

### Sprint 2
- build Home
- create core reusable components
- seed state model

### Sprint 3
- build Agent
- add tasks and approval states

### Sprint 4
- build Wallet
- integrate realistic mocked data model

### Sprint 5
- build Apps
- add launcher flow

### Sprint 6
- polish full flow
- create demo sequence

### Sprint 7
- draft bridge contract
- prepare packaging path

## Main risks
- scope inflation
- fake complexity instead of real coherence
- overusing blockchain in the product story
- building visuals without state logic
- building infrastructure before user experience is convincing

## Governing rule
If it does not make the MVP demo clearer, stronger, or more believable, it probably does not belong in the current sprint.
