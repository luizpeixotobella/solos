# SolOS Devlog

## 2026-04-02

- Confirmed local Qt version mismatch and resolved build path by pointing CMake to user-installed Qt.
- Diagnosed and fixed an early QML card layout issue causing bad sizing/overlap.
- Improved gateway access enough to inspect, edit, build, and commit the native shell.
- Established the documentation spine for the project:
  - README
  - thesis
  - architecture
  - roadmap
  - devlog
- Current implementation status: native shell scaffold with Home, Agent, Wallet, and Apps sections using mock data.
- Split the monolithic `Main.qml` into dedicated screen components:
  - `HomeScreen.qml`
  - `AgentScreen.qml`
  - `WalletScreen.qml`
  - `AppsScreen.qml`
- This establishes a cleaner path toward typed models and per-screen iteration.
- Began replacing ad hoc app strings with a dedicated `AppRegistryModel` and `AppTile` component so the Apps surface can evolve into a real module registry.
- Began the same transition on the Agent side with an `ActivityFeedModel` and `ActivityItem` component, turning the screen into the start of a real operational feed.
- Began the Home transition with a `QuickActionsModel` and `QuickActionTile`, replacing static quick-action copy with a first-class model-backed surface.
- Introduced a provisional `GhostRuntime`, an `ApprovalQueueModel`, and an `ApprovalItem` component so the Agent screen can start representing Ghost as a native shell presence instead of only as text.
- Next technical milestone: validate and extend the model-backed approach into richer wallet and approval-specific surfaces, then connect Ghost-facing models to real runtime data.
