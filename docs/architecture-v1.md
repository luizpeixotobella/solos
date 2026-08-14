# SolOS Architecture v1.0

## Objective

Define SolOS 1.0 as an operating layer that sits above a dedicated runtime intermediary, which itself sits above Linux.

This architecture supports:

- a believable MVP
- a coherent shell experience
- future integration with agents, wallets, and apps
- packaging into a Linux appliance and demo ISO

## Core correction

SolOS is **not** a new kernel.
Linux remains the base system.

But the runtime should also **not** be collapsed directly into Linux.

The correct architecture is:

- **Linux = base system layer**
- **runtime = intermediary mediation/orchestration layer**
- **SolOS = operating layer above that runtime**

This matters because the runtime has a distinct role:

- read Linux host capabilities and state
- normalize system facts into stable SolOS-facing contracts
- mediate services, processes, sessions, and capability boundaries
- shield the operating layer from raw host-specific details

So the runtime is neither:

- a fake replacement for Linux, nor
- merely another name for Linux itself

It is the middle layer between them.

## Layer model

### 1. Linux Base System Layer
Provides:
- kernel
- init system
- service manager
- filesystem
- process execution
- networking
- desktop/session primitives
- security boundaries

Examples:
- Linux kernel
- systemd
- X11 or Wayland session
- package/runtime stack

This is the host substrate on which SolOS depends.

### 2. SolOS Runtime Intermediary Layer
Provides:
- host-state discovery
- normalized runtime state and contracts
- service/process/session mediation
- capability boundaries
- orchestration semantics consumed by the operating layer

Current implementation direction:
- `app/runtime-core` in Rust reads Linux host facts and produces structured runtime state
- this layer should evolve from snapshot generation toward mediated services, events, and stable runtime contracts

This layer does not recreate Linux.
It also should not disappear conceptually into Linux.
It is the intermediary that turns Linux-host reality into SolOS-operable meaning.

### 3. SolOS Operating Layer
Provides:
- the SolOS experience
- Home, Agent, Wallet, and Apps surfaces
- approvals and task visibility
- navigation and ambient system presence
- user-facing policy and interaction grammar

Possible shell modes:
- native Qt/QML shell
- browser-based kiosk shell for demo packaging

This is the user-facing operating layer, built on top of the runtime intermediary.

### 4. Agent Bridge Layer
Provides:
- safe interface between agent actions and runtime-mediated capabilities
- approval workflow triggers
- bounded execution paths
- system-aware responses back to the shell

Principle:
- the agent does not directly become the runtime
- the agent acts through the runtime and explicit policy boundaries

### 5. Wallet / Identity Layer
Provides:
- profile identity
- wallet state
- signing visibility
- ownership-aware UX

Principle:
- wallet actions remain explicit
- signing is never hidden behind generic agent behavior
- wallet state is surfaced by SolOS through mediated runtime flows, not confused with kernel primitives

### 6. Apps Layer
Provides:
- app registry
- launch mediation
- app metadata and capability display
- local, web, dApp, and hybrid entry points

Principle:
- apps ultimately rely on host capabilities
- the runtime mediates those capabilities
- SolOS governs discovery, visibility, launch policy, and UX cohesion

## Ownership model

### Linux base system owns
- boot
- scheduler
- devices
- networking
- users/processes
- service lifecycle primitives
- session infrastructure primitives

### Runtime intermediary owns
- reading host runtime state
- normalizing host facts into SolOS runtime objects
- mediating selected Linux-backed actions
- exposing stable state/contracts/events to the operating layer
- reducing direct coupling between the shell and raw host-specific behavior
- running the owner-scoped SolOS Daemon for persistent state, health, local events, background coordination and future mediated action queues

Domain presentation remains in its natural SolOS space: Home, Ghost, Wallet, Apps or Pulso. Those surfaces consume Daemon contracts instead of embedding background/system service logic in QML or C++ UI code.

### Modularity invariant

SolOS is cohesive at the experience layer and modular at the implementation layer.

- The Daemon is one persistent process, not a fleet of premature microservices.
- Its internals are separated by domain ownership and typed/versioned contracts.
- An aggregated Home snapshot may summarize the environment, but it must not become the only internal interface between every domain.
- Shell controllers should coordinate presentation and requests; they must not absorb persistent Wallet, Ghost, quota or host-service behavior.
- Domain events may be joined for observability without erasing their source or policy boundary.

This modular-monolith rule is recorded in `docs/adr/0001-cohesive-shell-modular-core.md`.

### SolOS operating layer owns
- shell composition
- orchestration UX
- agent presence
- approval UX
- app registry model
- wallet/identity visibility
- user-facing policy and capability framing

## Runtime intermediary does not own

- kernel behavior
- init replacement
- a fake standalone operating-system runtime
- duplicate service/process semantics already handled by Linux

## Core data objects

### HostRuntime
- os
- kernel
- initSystem
- sessionType
- desktopSession
- shell
- hostname
- user

### RuntimeContext
- runtimeMode
- runtimeSource
- runtimeRole
- hostRuntimeSummary
- mediationStatus

### UserSession
- id
- displayName
- environmentMode
- lastActiveAt

### SystemStatus
- online
- version
- syncState
- notificationsCount
- approvalsCount
- hostRuntimeSummary

### AgentState
- status
- currentTask
- recentActions
- pendingApprovals

### WalletState
- connected
- address
- network
- balances
- assets
- pendingSignatureRequests

### AppDefinition
- id
- name
- type
- icon
- description
- capabilities
- status

## Security boundaries

### The agent may
- read allowed host/system state through the runtime intermediary
- suggest actions
- request bounded runtime-mediated operations
- open non-sensitive surfaces
- prepare workflows

### The agent may not autonomously
- sign transactions
- move assets
- install critical host components without confirmation
- access sensitive user data without explicit boundaries
- bypass Linux security or runtime mediation through SolOS abstractions

### The shell must always surface
- what action is being proposed
- whether the action touches runtime-mediated host services
- why approval is needed
- what the impact is

## Packaging principle for v1.0

SolOS v1.0 should ship first as a **Linux appliance with a SolOS runtime intermediary and operating layer**.

That means:
- boot a standard Linux distribution
- initialize runtime mediation above Linux
- auto-start the SolOS experience above that runtime
- keep responsibilities explicit across all three layers

This supports two parallel tracks:

1. **Native shell track**
   - Qt/QML shell as the long-term first-class SolOS operating layer
2. **Demo ISO track**
   - Linux image that boots directly into a SolOS demo environment

## Recommended implementation path

### v1.0
- keep Linux as the base substrate
- make runtime-core explicitly intermediary and host-aware
- remove language implying Linux itself is the entire runtime story
- remove language implying SolOS is inventing a replacement runtime
- package a demo ISO around Linux + runtime intermediary + SolOS shell

### v1.x
- replace more static snapshot fields with real host/service/app data
- continue splitting runtime-core into host, Ghost, Wallet, Apps, approvals, quota, events and contracts modules
- split the broad Qt AppController into domain-facing controllers without fragmenting the shell experience
- keep the aggregate snapshot for compatibility while adding smaller versioned domain state/events
- connect approvals to mediated commands and services
- add launcher bridge and app discovery from the host
- add real wallet/account brokers
- move from snapshot output toward stable runtime APIs/events

### later
- deepen session and display integration
- evolve the intermediary layer into a more durable local runtime service
- consider more dedicated appliance behavior once the operating layer is stable

## North-star outcome

A user boots a Linux-based SolOS image and feels they are inside a coherent operating environment where:
- Linux provides the base system
- the runtime intermediary translates and mediates system capabilities
- SolOS provides the operating layer and user experience
- the agent is native to the experience
- wallet and identity are visible
- apps feel like one system world
- approvals are explicit and understandable
- the architecture is honest about what belongs to Linux, to the runtime, and to the operating layer

## Founder utility seam

The fundraiser does not change the three-layer architecture. CMS/Supabase verifies support and issues a bounded entitlement; SolOS Wallet synchronizes it; runtime-core meters local Ghost usage. Stripe remains outside the operating layer, and provider-sponsored calls remain outside v1 until the signed-proof proxy is implemented. See `founder-rewards-contract.md`.
