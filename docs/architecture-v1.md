# SolOS Architecture v1

## Objective

Define the first serious system architecture for SolOS.

This architecture is designed to support:

- a believable MVP
- a coherent shell experience
- future integration with agents, wallets, and apps
- eventual packaging into a more system-like environment

## Architectural principle

SolOS should begin as an operating layer, not a new kernel.

That means:
- rely on a trusted base system underneath
- innovate in orchestration, UI, agent integration, and identity/app layers above it
- keep the future open for deeper system packaging later

## Core layers

### 1. Base System Layer
Provides:
- host OS services
- networking
- filesystem
- process execution
- security primitives

Near-term assumption:
- Linux is the long-term target environment
- development can occur from the current workstation setup

### 2. Shell UI Layer
Provides:
- the main SolOS experience
- navigation and screens
- panels, status, actions, transitions
- visual identity of the product

Primary screens:
- Home
- Agent
- Wallet
- Apps

### 3. State + Orchestration Layer
Provides:
- session state
- wallet state
- agent state
- apps registry state
- notifications
- approvals
- task lifecycle

This layer is what makes the UI feel like a system instead of static screens.

### 4. Agent Bridge Layer
Provides:
- safe interface between the agent and system capabilities
- action mediation
- approval workflow triggers
- system-aware responses back to the shell

Initial principle:
- the agent does not directly control everything
- the bridge exposes bounded capabilities

### 5. Wallet / Identity Layer
Provides:
- local profile identity
- SolOS profile state
- wallet connection state
- assets and signing visibility
- ownership-aware UX

Security principle:
- wallet signing must remain explicit and visible
- identity concepts should remain distinct unless intentionally linked

### 6. Apps Runtime Layer
Provides:
- app registry
- app metadata
- app type distinctions
- capability display
- app open/launch flow

App categories:
- local
- web
- dApp
- hybrid

## Core data objects

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
- read allowed system state
- suggest actions
- open non-sensitive surfaces
- prepare workflows

### The agent may not autonomously
- sign transactions
- move assets
- install critical components without confirmation
- access sensitive user data without explicit boundaries

### The shell must always surface
- what action is being proposed
- why approval is needed
- what the impact is

## Recommended implementation path

### MVP phase
- mocked state
- realistic UI
- typed services
- bridge contract drafted but partially simulated

### Next phase
- local bridge implementation
- limited agent integration
- early wallet integration

### Later phase
- app packaging
- VM packaging
- broader Linux environment integration

## Suggested code structure

```text
solos/
  docs/
  app/
    shell/
      src/
        app/
        components/
        features/
        state/
        services/
        styles/
        types/
```

Feature areas:
- `features/home`
- `features/agent`
- `features/wallet`
- `features/apps`

Service areas:
- `services/agent-bridge`
- `services/system`
- `services/wallet`
- `services/app-registry`

## North-star outcome

A user can open SolOS and feel they are inside a coherent operating environment where:
- the shell is intentional
- the agent is native
- wallet and identity are visible
- apps are part of one system world
- actions and approvals are understandable
