# SolOS Devlog

## 2026-04-03

- Re-read the thesis, architecture, roadmap, and launch collateral to keep implementation and narrative aligned.
- Reconfirmed the core external framing now present in the CMS launch pack: **SolOS as an operating layer for agents, dApps, and digital identity**.
- Added an explicit process rule to the working log: SolOS progress should be documented continuously in-repo rather than reconstructed later.
- Began a CMS alignment pass so project doctrine and outward-facing campaign language do not drift apart.
- Updated the matching CMS Business Affairs entry to include a live reference-doc section covering thesis, architecture, roadmap, devlog, launch pack, and the operating-layer visual.
- Reinforced a cross-system documentation rule: SolOS repo updates and CMS institutional updates should happen in the same operational cycle whenever the thesis or implementation meaningfully shifts.
- Began aligning the native shell with the newer public framing of “structure + something useful” by adding explicit Home-level environment guidance instead of leaving the surface as a generic section list.
- Added a first-pass “next useful action” concept to the native shell so Home can point toward the most important legible move rather than only describing the information architecture.
- Promoted Home guidance from ad hoc `AppController` strings into a dedicated `HomeState` object, moving the shell one step closer to a real structured Home model instead of a generic placeholder surface.
- Reframed the Home surface around a more legible system promise: show what matters now, then point toward the next useful safe action.
- Updated roadmap and architecture docs to reflect the current state more accurately, including the new `HomeState` direction and a selective Rust migration principle instead of a blanket rewrite posture.
- Created a matching documentation spine in `lbartes-cms/docs` so institutional/public framing can stay aligned with the SolOS repo.
- Registered a stronger documentation rule for the broader operation: SolOS changes should now be reflected in two synchronized tracks — offline docs in the repositories and official online documentation inside the webapp.

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
