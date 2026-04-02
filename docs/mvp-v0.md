# SolOS MVP v0

## Product goal

Create a first build that makes SolOS feel real.

The MVP does not need to be a full operating system. It needs to prove the product thesis through a coherent user experience.

## MVP user story

A user opens SolOS and can:

- enter a dedicated shell environment
- see their profile/session
- access a wallet-aware dashboard
- open a small catalog of apps
- talk to the built-in agent
- trigger at least one meaningful chain-aware workflow

## MVP screens

### 1. Welcome / Session screen
- enter SolOS
- choose local profile
- show basic system status

### 2. Home dashboard
- profile card
- wallet summary
- quick actions
- recent apps
- agent panel shortcut

### 3. Agent panel
- chat interface
- suggested actions
- system-aware prompts
- safe action confirmations

### 4. App launcher
- grid/list of available apps
- tags: local / web / dApp
- permission visibility

### 5. Wallet view
- connected account
- balances/assets
- recent actions
- signing request flow

## MVP tech suggestion

### Frontend shell
- React
- TypeScript
- Tailwind
- component library kept minimal

### Desktop wrapper
- Tauri preferred for efficiency
- Electron acceptable for speed

### Core services
- local Node.js service for early prototyping

### Solana
- `@solana/web3.js`

## MVP non-goals

Do not include in MVP:

- new kernel work
- full custom distro
- complex tokenomics
- permanent on-chain storage of all user data
- autonomous agent control without approval boundaries

## MVP success criteria

The MVP is successful if:

- it looks like a distinct product
- the shell is navigable
- wallet integration is demonstrable
- one app or workflow is genuinely useful
- the agent feels native to the environment
