# SolOS Architecture v0

## Goal

Define a realistic architecture for the first buildable version of SolOS.

The purpose of v0 is not to create a new operating system kernel. It is to create an operating layer that feels like a coherent system product.

## Architecture layers

### 1. Base System Layer

Responsible for:

- Linux base distribution
- networking
- storage
- process management
- security primitives
- packaging/runtime support

Initial recommendation:

- Ubuntu or Debian base for prototyping
- later, a custom image or distro variant

### 2. SolOS Shell Layer

Responsible for:

- desktop shell / launcher experience
- home screen / dashboard
- system navigation
- notifications
- user session entry point

Possible implementation:

- Tauri + React + TypeScript
- Electron + React + TypeScript if speed beats efficiency

### 3. Agent Layer

Responsible for:

- OpenClaw integration
- assistant UI inside the shell
- task execution bridge
- remote interaction through supported messaging channels
- orchestration of workflows

Design idea:

- local shell talks to a SolOS service bridge
- service bridge exposes safe operations
- OpenClaw acts as conversational and automation layer

### 4. Web3 Layer

Responsible for:

- Solana wallet connection
- signing flows
- asset/account views
- identity primitives
- trusted interaction with dApps

Initial recommendation:

- `@solana/web3.js`
- wallet abstraction layer
- support for local or embedded wallet UX depending on security design

### 5. App Runtime Layer

Responsible for:

- launching apps
- cataloging apps
- managing permissions
- exposing system capabilities to approved apps
- supporting local/web/dApp hybrid experiences

Concept:

- apps are not all equal
- some are local apps
- some are wrapped web apps
- some are dApps with wallet access
- SolOS should define a capability model for them

## Cross-cutting concerns

### Security

Must define clearly:

- what the agent can do automatically
- what requires explicit user approval
- what apps can access
- how wallet signing is separated from agent autonomy

### Identity

Need at least three concepts:

- local device user identity
- SolOS profile identity
- wallet identity

These may be linked, but should not be treated as identical by default.

### Data model

Use the right storage for the right job:

- local DB/config for app state and preferences
- encrypted local storage for sensitive material
- blockchain only for ownership, proofs, assets, or shared state where it truly matters
- optional off-chain storage for larger decentralized content

## Suggested service map

### `solos-shell`
User-facing shell UI.

### `solos-core`
Core local service coordinating shell, apps, permissions, and system APIs.

### `solos-agent-bridge`
Safe bridge between OpenClaw and local SolOS capabilities.

### `solos-wallet`
Wallet abstraction, signing requests, account state access.

### `solos-app-runtime`
App registry, launcher, capability model, app lifecycle.

## MVP architecture constraints

For the MVP, avoid:

- custom kernel work
- custom bootloader work
- trying to store everything on-chain
- full decentralization claims that the product does not actually fulfill

For the MVP, prioritize:

- coherent UX
- real wallet integration
- real agent integration
- a working launcher/dashboard
- VM-based demoability

## North-star outcome

A user can boot or launch SolOS, see a unified desktop-like environment, access wallet and identity, run at least one meaningful app, and interact with an agent that can help operate the environment.
