# SolOS Devlog

## 2026-07-14

- Documented **SolOS Pulso** as a planned social app surface for consented human signals, with a CMS public read-only preview at `/solos/pulso`.
- Added `docs/solos-pulso-credits.md` to define Pulso Credits as internal utility credit, not money, yield, passive income, or guaranteed revenue share.
- Updated roadmap and completion plan so Pulso is integrated with future Apps, Wallet, Ghost, Approvals, identity/Heart Pass, consent, and cost guardrails.
- Added Pulso to the app registry as a planned module so the shell can show the direction without claiming live data capture.

## 2026-07-12

- Re-read the SolOS documentation set, including product thesis, architecture, roadmap, devlog, Ghost docs, Heart Pass docs, cases, native migration notes, UI blueprint, and demo appliance docs.
- Re-read the matching LBArtes CMS documentation and online/executive SolOS surfaces so the product repo and CMS remain synchronized.
- Added `docs/product-completion-plan.md` as a living engineering plan for finishing SolOS v1.0, with current state, done/partial/pending checklist, prototype board, milestones, verification gates, and discussion decisions for Luiz.
- Recorded the recommended next engineering slice as **Ghost Trace Persistence and Evaluation Seed**, because the last local implementation already exposes request classification, action trace, and route explanation but does not yet persist outcomes.
- Updated the roadmap and README so future work starts from the completion plan instead of reconstructing status from scattered docs.

## 2026-07-08

- Promoted the archived W3Schools AI/Machine Learning notes into the first concrete AI Ghost implementation slice.
- Added `ghost.requestClassifier`, `ghost.actionTrace`, and `ghost.routeExplanation` to the runtime snapshot so Ghost can show class, safety, tools, approval needs, quota cost, selected route, and outcome before implying execution.
- Extended the Qt runtime bridge and `GhostRuntime` with classification/trace/route properties, then rendered the new cards in the native Agent/Ghost screen.
- Mirrored the same classification and trace concept in the web/mock Agent surface so prototype and native shell stay aligned.
- Documented the slice in `docs/cases/004-ai-ghost-classification-trace.md` and updated the W3Schools archive status.
- Re-read the SolOS repository docs, the CMS docs/public content, and the CMS private Executive pages before continuing the next implementation pass.
- Implemented the first local **Heart Pass Quota Layer** slice: `config/heart_pass.json` now carries a `quotaLayer` contract with planned hybrid sponsored/BYOK mode, included/used/remaining query counts, fallback, usage source, sync state, and reset policy.
- Extended `runtime-core` so `heartPass.quotaLayer` is emitted in the runtime snapshot and visible in the activity feed as a real runtime contract, while remaining disabled until Heart Pass ownership is verified.
- Extended the Qt runtime bridge and `AppController` with quota properties, then rendered Heart Pass Quota cards in both Wallet and Agent/Ghost surfaces.
- Regenerated `app/shell-native/src/runtime_snapshot.json` and rebuilt the native shell successfully.

## 2026-06-19

- Re-checked the Heart Pass documentation and interpreted the next product step as the **Heart Pass Quota Layer**: a transparent monthly usage allowance that converts verified ERC-1155 ownership into measurable Ghost research utility.
- Added `docs/cases/003-heart-pass-quota-layer.md` to specify the hybrid sponsored-quota + BYOK fallback architecture, runtime contract draft, supervision requirements, public wording, and staged implementation plan.
- Updated `docs/solos-heart-pass-brave-credits.md` so the current state now points from wallet verification and Ghost gating toward quota visibility before any backend spending is introduced.

## 2026-05-06

- Published the first CMS blog post for the pass, making the Brave/Search ownership model publicly visible while keeping the internal SolOS documentation aligned.
- Drafted the **SolOS Heart Pass** as the first utility-pass framing for SolOS, anchored by the existing Polygon NFT `Anastacia Our Hearts #1`.
- Documented the initial Brave Search credit strategy: keep the default model user-owned/BYOK, validate and store each user's Brave key locally, and treat any future sponsored credits as a backend-enforced quota layer rather than a shared client key.
- Clarified the MetaMask role: wallet ownership/signatures can prove pass ownership or accept explicit payments, but MetaMask should not be treated as a silent SaaS-billing API for Brave/OpenAI credits.
- Added `docs/solos-heart-pass-brave-credits.md` so the pass concept, Brave pricing assumptions, per-user quota model, and non-financial-product language stay documented before public posting.


## 2026-05-01

- Clarified the intended meaning of Ghost's intelligence formula: classical software follows `algorithms + data = results`, while Ghost should develop from the AI-era synthesis `data + results = algorithms`.
- Added `docs/ghost-intelligence-doctrine.md` to record the conceptual lineage from Rosenblatt's perceptron toward layered Ghost behavior, while preserving a practical distinction between rule-based AI and machine learning.
- Reframed Ghost's runtime pipeline as data, results, algorithms, and knowledge layers so the implementation continues from this doctrine rather than treating the phrase as decorative copy.
- Aligned the SolOS README, architecture notes, and runtime-core README with this direction so future Ghost/SolOS work keeps the same product thesis.

## 2026-04-24

- Re-read the current SolOS thesis, architecture, roadmap, README, and aligned LBArtes CMS documentation before continuing development, reinforcing the standing rule to document while building.
- Advanced `app/runtime-core` from a mostly declarative host snapshot toward a more real runtime seam by adding live host facts for hostname, user, kernel, uptime, session type, and coarse online/offline detection.
- Added a `systemStatus` contract so the shell now receives mediated runtime state for online status, approval count, notification count, and a host-runtime summary instead of inferring everything from loose copy.
- Upgraded approval modeling from a thin queue item into a more system-like primitive with `id`, `description`, `requestedBy`, `capability`, `status`, and `createdAt`, while keeping the initial entries mock-backed but structurally honest.
- Extended the Qt runtime bridge and `AppController` so the native shell can ingest the richer runtime contract rather than treating approvals and host state as shallow labels.
- Regenerated the runtime snapshot from the Rust runtime and rebuilt the native shell successfully after the contract expansion.
- Continued immediately into the shell layer so the new runtime truth became visible: Home now surfaces host runtime summary, online state, approval count, and runtime event count, while Agent approvals now render requester, capability, description, status, and creation metadata instead of only title/scope/risk.
- Turned the provisional Ghost presence into the first functional Ghost prototype path: a layered intelligence pipeline with `data`, `results`, and `algorithms` stages, exposed through runtime-core and rendered in the native Agent surface.
- Added optional Brave-grounded research to Ghost, then corrected the ownership model so SolOS no longer assumes a shared developer key for public use.
- Created a repository-local Ghost config at `solos/config/ghost.json` and established a new rule: Ghost web research must rely on the user's own Brave key, not the developer's global config.
- Implemented Ghost onboarding inside the Agent surface so users are sent to Brave's API-key page, return to SolOS, paste their own key, validate it live, and only then persist it into the repo-local config.
- Added safe reset behavior so the repo can remain in a bootstrap state with no embedded Brave key, preserving the new ownership and billing boundary.
- Documented this normative shift explicitly in `docs/ghost-onboarding-policy.md` so future Ghost work keeps account ownership, approvals, and external-cost boundaries legible.

## 2026-04-19

- Corrected an architectural inconsistency: SolOS had first been reframed as an operating layer above Linux, but that still collapsed the runtime semantics too directly into the host system.
- Refined the model again to the more precise three-layer posture: **Linux base system -> runtime intermediary -> SolOS operating layer**.
- Repositioned `app/runtime-core` as the first real **runtime intermediary** boundary, responsible for reading Linux host facts, mediating them, and exposing stable SolOS-facing runtime state.
- Added explicit runtime-role and mediation-status fields to the runtime snapshot so the shell can distinguish Linux substrate, runtime mediation, and operating-layer semantics.
- Updated architecture, version notes, README language, and shell fixture copy to make the ownership boundary unambiguous across all three layers.
- Kept the demo ISO path aligned with that correction so the packaging story now reflects Linux base system + runtime mediation + SolOS operating layer.

## 2026-04-13

- Reassessed the current SolOS implementation in plainer terms so the documentation reflects the code more honestly.
- Clarified that the shell is no longer just a pile of ad hoc strings, but also not yet a deeply dynamic system.
- Documented the present execution path more explicitly: Rust emits a structured runtime snapshot, C++ imports and coordinates it, Qt models/state objects hold it, and QML renders it.
- Recorded an important product/architecture correction: the next step is not a blanket rewrite, but turning the current structured snapshot seam into progressively real runtime state.
- Updated roadmap language to make the guinada explicit: fewer semi-static staging controllers over time, more real runtime-derived state, approvals, wallet/account data, and app capability boundaries.
- Synchronized this clarification pass with the LBArtes CMS documentation so internal and external explanations stay aligned.

## 2026-04-10

- Defined the first real multi-language boundary for SolOS: runtime snapshot generation on one side, native shell presentation on the other.
- Added `runtimebridge.*` to the Qt/C++ shell so structured state can replace indefinitely hardcoded UI copy.
- Made `HomeState`, `GhostRuntime`, and list-backed models mutable from imported runtime data.
- Scaffolded `app/runtime-core` as the first Rust subsystem candidate.
- Updated SolOS docs and LBArtes CMS surfaces so architecture direction, public framing, and implementation roadmap stay aligned.
- Reinforced a working product rule: each implementation pass should improve structure and also create a useful visible change in the shell.
- Added live refresh behavior so Home, Agent, and Apps can react to runtime snapshot changes while the shell is open.

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
# 2026-06-12 - Ghost operational readiness

- Added `ghost.operationalReadiness` to the runtime-core snapshot.
- Exposed readiness status, summary, and pillar lines through the C++ runtime bridge and `GhostRuntime`.
- Added an Operational Readiness card to the native Agent/Ghost screen.
- Documented the readiness model in `docs/ghost-operational-readiness-2026.md`.
- Readiness pillars now track grounded research/RAG, long-term memory, tool/MCP boundary, approval lane, observability/evals, and language mediation.
- Public CMS docs and blog/social materials were prepared in the LBArtes CMS to keep the external narrative aligned with the SolOS repo.
