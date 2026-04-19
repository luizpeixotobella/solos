# SolOS Architecture

## Current prototype

The current native shell is a Qt/QML application with a C++ coordination layer and an early Rust runtime-state generator.

### Present structure

- `app/shell-native/src/main.cpp`
  - bootstraps `QGuiApplication`
  - loads the QML module
  - injects the global `AppController` into the QML context

- `app/shell-native/src/appcontroller.{h,cpp}`
  - coordinates top-level shell state
  - owns current screen, runtime labels, refresh loop, and model objects
  - imports runtime snapshot data through `runtimebridge.*`
  - distributes structured state into Home, Agent, Approval, and Apps surfaces

- `app/shell-native/src/runtimebridge.*`
  - parses the runtime snapshot contract
  - maps imported JSON into C++/Qt model data

- `app/shell-native/qml/Main.qml`
  - defines the main shell window
  - contains sidebar, top status bar, and screen stack

- `app/shell-native/qml/components/*`
  - reusable shell components

- `app/runtime-core/src/main.rs`
  - emits a structured runtime snapshot in JSON
  - currently acts as the first Rust subsystem boundary for SolOS

## Immediate architectural direction

### 1. Move from hardcoded strings to typed models

The shell should stop hardcoding most UI content in QML and instead expose typed state from C++.

This transition is now visibly underway:

- `AppRegistryModel`
- `ActivityFeedModel`
- `QuickActionsModel`
- `ApprovalQueueModel`
- `HomeState`

Candidate models:

- navigation model
- app registry model
- quick actions model
- approval queue model
- activity feed model

### 2. Separate shell surfaces from data sources

The QML should describe presentation. C++ should own application state and integration points.

### 3. Introduce reusable screen components

Instead of keeping all screens inside `Main.qml`, split into:

- `screens/HomeScreen.qml`
- `screens/AgentScreen.qml`
- `screens/WalletScreen.qml`
- `screens/AppsScreen.qml`

### 4. Design around first-class system concepts

The system should model:

- sessions
- agents
- tasks
- approvals
- wallets
- apps
- capabilities
- recent actions

## Proposed near-term component map

### C++ layer

- `AppController`
- `NavigationModel`
- `AppRegistryModel`
- `ApprovalQueueModel`
- `ActivityFeedModel`
- `QuickActionsModel`
- `GhostRuntime`

The first step in this direction is now in progress: the Apps surface is moving from a plain string list to a dedicated `AppRegistryModel` backed by `QAbstractListModel`.

The same model-backed approach is now starting in the Agent surface through an `ActivityFeedModel`, which should evolve into a real event and approval feed.

Home is also beginning the transition via a `QuickActionsModel`, which gives the shell a concrete path from static messaging toward executable system actions.

`HomeState` now adds a second layer to Home: a structured summary of the environment plus a legible “next useful move” concept. This is important because SolOS should not just describe sections — it should bias the system toward safe, understandable action.

A provisional `GhostRuntime` now defines the beginning of a native agent presence inside the shell, alongside an `ApprovalQueueModel` that can later be connected to real approval requests.

### QML layer

- shell chrome
- reusable cards
- lists and status surfaces
- modal / approval surfaces
- screen-specific components

## Integration philosophy

The shell should eventually become a local orchestrator surface over real services, not a fake dashboard. Mock data is fine in the earliest stage, but every mock should point toward a real model boundary.

## Selective Rust direction

Rust should be introduced where subsystem boundaries become strong enough to justify it.

Likely candidates:

- background runtime services
- approval/task orchestration components
- parsers, capability layers, and security-sensitive logic
- future system modules that benefit from stronger safety guarantees

The Qt/QML shell itself can continue iterating with a thin C++ layer until real integration boundaries are stable. The architectural goal is not language purity; it is a safer and more coherent system.

## What the code is doing right now

At a high level, the current SolOS shell works like this:

1. Rust generates a structured runtime snapshot.
2. C++ loads and interprets that snapshot.
3. `AppController` pushes the imported state into Qt models and state objects.
4. QML renders those models as native shell surfaces.

So the present codebase is no longer only a pile of static strings manually pasted into the UI.

It is also not yet a full dynamic operating system.

The most accurate description is:

- **partially structured**
- **still prototype-heavy**
- **already moving toward real subsystem boundaries**

This distinction matters.

A lot of visible content is still effectively static in product terms because the Rust side currently emits predefined snapshot values rather than live system integrations. But the architectural move has already started: state is now being shaped as a contract, imported by the shell, and distributed through models instead of remaining forever as scattered UI literals.

## First live boundary: runtime snapshot contract

The current implementation now has a concrete first seam for multi-language architecture:

- `app/runtime-core` (Rust) can produce structured runtime state
- `app/shell-native/src/runtimebridge.*` (C++) parses and imports that state
- QML consumes model-backed shell state instead of expanding hardcoded copy forever

This boundary is intentionally modest.

It is useful because it lets SolOS migrate real system meaning before it migrates everything else.

### Why this seam matters

It creates an immediate path for:

- runtime health and environment summaries
- next-action recommendations
- task and approval feeds
- wallet/account visibility
- future capability and orchestration state
- visibly refreshed shell state while the app is running

This last point matters product-wise: SolOS implementation should not only improve architecture, it should also create perceivable movement in the shell. Better structure and visible usefulness should advance together.

## Honest assessment of the current limitations

The current runtime contract is still modest.

Today, most of the generated state is predetermined snapshot content, not a broad live operating substrate. In practice, that means:

- the Rust side is generating structured data, but not yet deep real-world system state
- the C++ side is coordinating real shell surfaces, but many surfaces still depend on prototype assumptions
- the QML layer is becoming cleaner and more model-driven, but the system is not yet broadly dynamic

So the next step should not be a theatrical rewrite.

The next step should be to make the runtime contract progressively more real.

That means growing from:

- static snapshot content

toward:

- runtime-derived state
- executable approval flows
- real task/activity updates
- wallet/account integration
- launcher and capability boundaries for apps

### Language roles right now

#### C++ / Qt / QML

Owns:

- native shell windowing
- interaction grammar
- model exposure to QML
- fast iteration on OS surfaces

#### Rust

Owns first:

- structured runtime state generation
- future orchestration services
- approval/task backends
- safety-sensitive parsing and system logic

#### JSON snapshot contract

Temporarily owns:

- low-friction interchange between the two layers
- inspectable, testable, debuggable integration seam

Later this can evolve into:

- local service IPC
- FFI boundary
- typed schema-validated transport
