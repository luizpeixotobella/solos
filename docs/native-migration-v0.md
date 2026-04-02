# SolOS Native Migration v0

## Why this document exists

SolOS already has two credible assets:

- a **web shell prototype** in `app/shell`
- a **Linux demo appliance path** in `appliance/demo-linux-v1`

That is enough to prove product direction, but not enough for the long-term runtime story.

The current browser-kiosk approach is useful because it ships quickly and keeps the UX visible. It should now be treated as the **UX and interaction reference**, not the final architectural destination.

## Current product and architecture state

### Product state

SolOS currently presents itself as:

- a Linux-aligned operating environment
- an agent-native shell with Home / Agent / Wallet / Apps
- a local-first system with wallet, identity, and app concepts exposed as first-class surfaces
- an environment that can later be packaged into a more appliance-like or distro-like form

### Implementation state in this repository

#### 1. Web shell prototype
Location: `app/shell`

What exists today:

- Vite + React + TypeScript shell
- reusable screen/component structure
- mocked system, wallet, app, conversation, approval, and task state
- a guided demo flow that makes the shell feel system-like rather than static

What it currently does well:

- demonstrates the SolOS UX shape
- tests information architecture
- encodes the first believable agent approval flow
- gives a concrete reference for visual hierarchy and interaction tone

What it does not yet do:

- run as a native Linux shell
- own the desktop/session lifecycle
- integrate with real system services, wallet backends, or an agent bridge
- control compositor/windowing behavior or system surfaces directly

#### 2. Linux demo appliance scaffold
Location: `appliance/demo-linux-v1`

What exists today:

- a Debian/Ubuntu-style kiosk demo path
- local static serving of `app/shell/dist`
- Chromium kiosk launch scripts
- systemd/X11 scaffolding for a boot-to-demo machine or VM

What it currently proves:

- SolOS can be packaged into a believable Linux appliance demo quickly
- the product story works in a bootable environment
- the repo is already oriented toward Linux as the destination platform

What it does not solve:

- native rendering
- system-owned shell orchestration
- direct process/session/window management
- tighter integration between UI and host capabilities

## Migration rationale: why move toward a native shell

A browser shell is the right prototype medium, but a weak final substrate for an operating environment.

### Limits of the web-shell-as-runtime approach

- browser kiosk mode is still a browser pretending to be a shell
- deeper Linux integration becomes awkward and fragile
- system surfaces, launch behavior, focus, and lifecycle are indirect
- desktop-shell responsiveness and polish are harder to guarantee
- security boundaries for wallet, agent, and privileged actions become more convoluted

### Advantages of a native Linux-aligned shell

A native shell can:

- own the top-level windowing/shell experience directly
- integrate with Linux services, IPC, session state, and process management more cleanly
- separate presentation from host capabilities with stronger contracts
- reduce dependence on Chromium for the core environment
- become the foundation for a true appliance shell later

## Preferred target: Qt/QML + C++

Qt/QML + C++ is the preferred next step because it matches the product shape well.

### Why Qt/QML fits SolOS

- strong support for Linux desktop and embedded/appliance environments
- QML is excellent for shell-style, panel-oriented, animated interfaces
- C++ gives a robust systems-facing layer for process, IPC, service, and platform integration
- the stack can scale from prototype shell to kiosk/appliance runtime without changing UI technology again
- it leaves room for Wayland/X11 integration decisions later

### Recommended responsibility split

#### QML
Use for:

- shell composition
- navigation and layout
- status surfaces
- animated transitions
- cards, rails, lists, and top-level screen structure

#### C++
Use for:

- application bootstrap
- state models exposed to QML
- Linux/system integration
- app registry and launch mediation
- agent bridge boundary
- wallet and identity bridge adapters

## Proposed transition model

Do not rewrite blindly. Port with intent.

### Phase 1: preserve product logic, swap runtime direction

Use the existing web shell as the reference for:

- information architecture
- screen hierarchy
- visual rhythm
- approval/task/app semantics

Create a native shell skeleton that mirrors the same concepts:

- App shell
- Home
- Agent
- Wallet
- Apps
- status/top bar
- agent pulse / approval visibility

### Phase 2: introduce native state models

Replace mocked frontend-only state with C++/QML models for:

- session state
- system status
- agent state
- wallet state
- app registry
- approvals/tasks/notifications

### Phase 3: bridge into host services

Add bounded adapters for:

- Linux session/process launch
- app discovery and activation
- OpenClaw agent bridge
- wallet provider integration
- secure approval workflows

### Phase 4: converge the appliance story

Once the native shell is functional:

- run it as the primary shell window in the Linux demo image
- reduce the browser to a compatibility surface for embedded web apps, not the main shell
- evolve the appliance toward a more self-contained SolOS environment

## Initial native repository direction

Recommended location:

- `app/shell-native`

Recommended structure:

```text
app/shell-native/
  CMakeLists.txt
  README.md
  src/
    main.cpp
    appcontroller.h
    appcontroller.cpp
  qml/
    Main.qml
    components/
      SidebarNav.qml
      TopStatusBar.qml
      SectionCard.qml
  assets/
```

## Early architectural assumptions

- Linux remains the target runtime even if development happens on Windows/macOS first
- the current native scaffold is intentionally mock-data-first
- QML reproduces the shell shape first; real host bindings come after
- Wayland support should be considered the long-term preferred Linux display path, but the scaffold should stay generic initially

## What should be ported first from the web shell

Port these concepts first, not every implementation detail:

1. screen map: Home / Agent / Wallet / Apps
2. top status bar with session/system/wallet summary
3. sidebar navigation
4. approval/task visibility
5. app list with activation state
6. ambient agent presence

## What should not be ported too literally

- web-specific component plumbing
- React state hooks as architecture
- browser-serving assumptions
- Chromium kiosk constraints as product constraints

## Immediate next step after this document

Create the initial Qt/QML + C++ shell scaffold and keep it explicitly aligned with the current web-shell semantics.
