# SolOS Architecture v1.0

## Objective

Define SolOS 1.0 as an operating layer that runs on top of Linux instead of pretending to replace Linux with a separate runtime.

This architecture supports:

- a believable MVP
- a coherent shell experience
- future integration with agents, wallets, and apps
- packaging into a Linux appliance and demo ISO

## Core correction

SolOS is **not** a new kernel and should **not** invent a parallel runtime when Linux already provides:

- process scheduling
- memory management
- device drivers
- networking
- filesystem semantics
- init and service management
- session and desktop plumbing

So the architecture rule for v1.0 is simple:

**Linux owns the runtime. SolOS owns the operating layer above it.**

Rust, C++, QML, and web surfaces are implementation tools for the SolOS layer, not replacements for the Linux runtime.

## Layer model

### 1. Linux Host Runtime Layer
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
- local package/runtime stack

This is the real runtime substrate.

### 2. SolOS Runtime Adapter Layer
Provides:
- host-state discovery
- normalized runtime snapshot/state
- service mediation
- capability boundaries
- orchestration contracts exposed to the shell

Current implementation direction:
- `app/runtime-core` in Rust reads Linux runtime facts and produces structured state
- this layer should evolve from snapshot generation toward real mediated services and event streams

This layer does not recreate Linux. It translates Linux-host reality into SolOS concepts.

### 3. SolOS Shell UI Layer
Provides:
- the SolOS experience
- Home, Agent, Wallet, and Apps surfaces
- approvals and task visibility
- navigation and ambient system presence

Possible shells:
- native Qt/QML shell
- browser-based kiosk shell for demo packaging

The shell is the user-facing operating layer, not the underlying runtime.

### 4. Agent Bridge Layer
Provides:
- safe interface between agent actions and Linux-backed capabilities
- approval workflow triggers
- bounded execution paths
- system-aware responses back to the shell

Principle:
- the agent does not directly become the runtime
- the bridge mediates access to Linux-backed operations

### 5. Wallet / Identity Layer
Provides:
- profile identity
- wallet state
- signing visibility
- ownership-aware UX

Principle:
- wallet actions remain explicit
- signing is never hidden behind generic agent behavior
- wallet state is surfaced by SolOS, but not confused with kernel-level primitives

### 6. Apps Layer
Provides:
- app registry
- launch mediation
- app metadata and capability display
- local, web, dApp, and hybrid entry points

Principle:
- apps run using host capabilities
- SolOS governs discovery, visibility, launch policy, and UX cohesion

## Runtime ownership model

### Linux owns
- boot
- scheduler
- devices
- networking
- users/processes
- service lifecycle
- session infrastructure

### SolOS owns
- shell composition
- orchestration semantics
- agent presence
- approval UX
- app registry model
- wallet/identity visibility
- user-facing policy and capability framing

### Rust runtime-core owns
- reading host runtime state
- normalizing host facts into SolOS state
- mediating selected Linux-backed actions
- eventually exposing evented contracts to the shell

### Rust runtime-core does not own
- kernel behavior
- init replacement
- a fake full-system runtime
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
- read allowed host/system state
- suggest actions
- request bounded Linux-backed operations
- open non-sensitive surfaces
- prepare workflows

### The agent may not autonomously
- sign transactions
- move assets
- install critical host components without confirmation
- access sensitive user data without explicit boundaries
- bypass Linux security and service boundaries through SolOS abstractions

### The shell must always surface
- what action is being proposed
- whether the action touches Linux-host services
- why approval is needed
- what the impact is

## Packaging principle for v1.0

SolOS v1.0 should ship first as a **Linux appliance layer**.

That means:
- boot a standard Linux distribution
- auto-start the SolOS experience
- use Linux services below
- keep SolOS focused on orchestration and user experience above

This supports two parallel tracks:

1. **Native shell track**
   - Qt/QML shell as the long-term first-class SolOS shell
2. **Demo ISO track**
   - Linux image that boots directly into a SolOS demo environment

## Recommended implementation path

### v1.0
- keep Linux as the runtime substrate
- make runtime-core explicitly host-aware
- remove language implying SolOS is inventing its own runtime
- package a demo ISO around Linux + SolOS shell

### v1.x
- replace more static snapshot fields with real host/service/app data
- connect approvals to mediated commands and services
- add launcher bridge and app discovery from the host
- add real wallet/account brokers

### later
- move from snapshot seam toward events/IPC
- deepen session and display integration
- consider more dedicated appliance behavior once the operating layer is stable

## North-star outcome

A user boots a Linux-based SolOS image and feels they are inside a coherent operating environment where:
- Linux provides the execution substrate
- SolOS provides the shell and orchestration layer
- the agent is native to the experience
- wallet and identity are visible
- apps feel like one system world
- approvals are explicit and understandable
- the architecture is honest about what is runtime and what is operating layer
