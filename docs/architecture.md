# SolOS Architecture

## Current prototype

The current native shell is a Qt/QML application with a thin C++ controller.

### Present structure

- `app/shell-native/src/main.cpp`
  - bootstraps QGuiApplication
  - loads the QML module
  - injects `appController` into the QML context

- `app/shell-native/src/appcontroller.{h,cpp}`
  - exposes top-level shell state
  - current screen
  - mock labels for session, system, wallet, and agent
  - mock list of app names

- `app/shell-native/qml/Main.qml`
  - defines the main shell window
  - contains sidebar, top status bar, and screen stack

- `app/shell-native/qml/components/*`
  - reusable shell components

## Immediate architectural direction

### 1. Move from hardcoded strings to typed models

The shell should stop hardcoding most UI content in QML and instead expose typed state from C++.

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

A provisional `GhostRuntime` now defines the beginning of a native agent presence inside the shell, alongside an `ApprovalQueueModel` that can later be connected to real approval requests.

### QML layer

- shell chrome
- reusable cards
- lists and status surfaces
- modal / approval surfaces
- screen-specific components

## Integration philosophy

The shell should eventually become a local orchestrator surface over real services, not a fake dashboard. Mock data is fine in the earliest stage, but every mock should point toward a real model boundary.
