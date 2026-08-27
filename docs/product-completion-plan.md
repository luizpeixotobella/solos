# SolOS Product Completion Plan

Date: 2026-08-23

Status: v1.0 RC2 candidate plus a fail-closed Pulso Alpha 0 P0 implementation; Ghost reviewer validation and Pulso operational readiness remain separate gates.

## Purpose

This document turns the current SolOS state into a completion plan that can be tracked, discussed, prototyped, verified, and shipped.

The goal is not to inflate SolOS into a finished custom operating system too early. The goal is to finish a credible v1 product path:

> SolOS v1.0 is a Linux-based operating layer above a runtime intermediary, with Home, Agent, Wallet, Apps, approvals, Ghost, and Heart Pass utility moving from prototype surfaces into observable runtime-mediated behavior.

## Current position

### Already done

- [x] Product thesis documented in `docs/thesis.md`.
- [x] Architecture corrected into three layers: Linux base system, runtime intermediary, SolOS operating layer.
- [x] Native shell scaffold created in Qt/QML with C++ controller/model bridge.
- [x] Core screens exist: Home, Agent, Wallet, and Apps.
- [x] Monolithic shell UI split into dedicated QML screen components.
- [x] Initial model-backed surfaces created for apps, activity, quick actions, approvals, and Ghost runtime state.
- [x] Rust `runtime-core` created as the first runtime intermediary boundary.
- [x] Runtime snapshot emits host/runtime context instead of only static UI text.
- [x] Runtime snapshot includes live host facts such as OS, kernel, hostname, user, uptime, session type, and coarse online state.
- [x] `systemStatus` exists as a structured runtime contract.
- [x] Approvals now carry requester, capability, status, description, and creation metadata.
- [x] Ghost has a documented intelligence doctrine: `data + results = algorithms`.
- [x] Ghost web research onboarding uses repository-local user-owned Brave key configuration instead of a shared developer key.
- [x] Heart Pass verification direction is documented around Polygon ERC-1155 ownership.
- [x] Heart Pass Quota Layer exists as a local runtime/UI contract.
- [x] Wallet and Agent/Ghost surfaces render Heart Pass quota status, usage, fallback, and verification requirement.
- [x] Ghost operational readiness exists in runtime and native Agent/Ghost UI.
- [x] Ghost request classification, action trace, and route explanation exist in runtime, native UI, and web/mock UI.
- [x] SolOS Pulso and Pulso Credits are documented as a future social/credit app surface.
- [x] Demo ISO / Linux appliance path is documented and scaffolded.
- [x] CMS documentation, blog, social copy, and executive pages are aligned with the Heart Pass quota-runtime public narrative.
- [x] Persistent SolOS Daemon serves health, snapshots and bounded local events over an owner-only Unix socket.
- [x] First runtime-core domains extracted into host discovery and operating-surface catalog modules without breaking the RC1 snapshot contract.
- [x] Web and native executable smoke gates cover repository asset serving, early shell exit and QML runtime errors.
- [x] Home and Agent QML delegate bindings corrected and verified in a real offscreen launch.
- [x] Ghost Resolution Loop selects one objective, builds a bounded plan, stops for approval, executes `app.open.safe`, verifies the Workspace state and retains evidence.
- [x] Ghost resolution state is versioned, atomically persisted by the Daemon and injected into the RC1 snapshot read model.
- [x] Native and web Agent surfaces expose the selected goal, progress, ordered steps, approval decision, result and unavailable future candidates.
- [x] Ghost Audit Challenge accepts real input, keeps embedded instructions inert, writes one approval-bound Linux artifact and produces a portable receipt through a separate verifier process.
- [x] Ten-reviewer pilot contract, GitHub return form and acquisition baseline exist for testing usefulness, return behavior and concrete support intent.

### Partially done

- [~] Home is model-backed enough to show environment summary and next useful action, but not yet driven by a broad live event stream.
- [~] Agent now persists trace outcomes and completes one real bounded resolution route; general task execution, research and public-send routes remain unavailable.
- [~] Ghost now has a real-input integrity/safety audit pilot, but its deterministic classification still requires human evaluation and the local receipt is not remote attestation.
- [~] Wallet shows pass/quota semantics, but real wallet/account state remains limited to the current Heart Pass path.
- [~] Apps has registry-style structure, but not yet a real launcher/capability bridge.
- [~] Pulso now has a real CMS/server/database Alpha 0 contract for verified-adult invites, text submission, human premoderation, bounded feed, export/delete and operator evidence, but all gates remain closed and it is not yet a native SolOS app.
- [~] Runtime state is structured and partly host-derived, but still depends on snapshot output rather than a durable service/API/event layer.
- [~] A persistent Rust Daemon now provides an owner-only Unix socket, health, RC1 snapshot compatibility, bounded local events and the first durable domain store for Ghost resolutions; broader durable event/domain stores remain next work.
- [~] Demo appliance documentation exists, but the current native shell has not yet replaced the browser-kiosk path as the tested primary demo session.
- [~] CMS and public narrative are aligned through the last quota-runtime cycle, but now need a standing completion dashboard rather than only campaign updates.
- [~] Runtime-core now has its first internal modules, while Ghost, Wallet/quota and contract assembly still need domain extraction.
- [~] The Qt shell is split into screens/models, while `AppController` remains broader than the desired domain-controller boundary.

### Not done yet

- [ ] Collect ten independent Ghost Audit receipts plus human usefulness verdicts; implementation is ready, cohort progress is 0/10 before public release.
- [ ] Validate repeat use and concrete support intent before claiming fundraising conversion.
- [x] Persist Ghost trace outcomes and accepted/rejected/corrected examples.
- [x] Add versioned schema invariants and tests for the runtime snapshot contract.
- [x] Add a typed tool/capability manifest with read, write, sensitive, network, wallet, and public-posting scopes.
- [x] Connect approvals to the first safe mediated action (`app.open.safe`) in the demonstrator.
- [x] Link objective selection, plan, approval, mediated action, verification and retained evidence in one complete Ghost resolution.
- [ ] Connect Ghost research usage to quota accounting.
- [x] Define the provider-neutral server-side quota/proxy contract for sponsored usage; runtime remains disabled until a signed-proof backend exists.
- [ ] Define Pulso Credits ledger, caps, anti-fraud, expiry, and cost guardrails.
- [x] Add Pulso Alpha 0 export/delete/moderation and fail-closed invite/adult gates before real social capture.
- [x] Require signed holder/session proof before sponsored quota calls.
- [x] Define a provider-neutral retrieval boundary; Brave remains the first adapter.
- [x] Add scoped Ghost memory classes with revocation semantics.
- [x] Expose honest Wallet/session proof state beyond UI-only assumptions.
- [x] Add an app launcher bridge in the demonstrator and capability declarations for runtime mediation.
- [ ] Formalize reusable shell theme tokens and scroll behavior for content-heavy screens.
- [ ] Build and smoke-test the demo ISO path in a VM.
- [x] Publish a repeatable v1 demo script and release checklist.
- [ ] Split the broad Qt controller into runtime, Ghost, Wallet and Apps-facing controllers/services.
- [~] Move durable domain state/events behind Daemon-owned stores instead of treating the aggregate snapshot as the only internal bus; Ghost resolutions and real-input audits are the first completed stores.

## Recommended next product slice

The next slice should be **Release and Measure the Ten-Reviewer Ghost Audit Pilot**. Quota accounting and launcher hardening remain the next engineering work after this validation gate produces real reviewer evidence.

Reason:

- GitHub already reports 24 clones and 17 unique cloners in the current rolling traffic window, but no identified reviewer or revenue conversion yet.
- The audit kit now gives those cloners one concrete exchange: run their own input, receive a reproducible receipt and judge Ghost's classification.
- More feature claims before return/usefulness/payment evidence would repeat the old marketing-first error.
- Ten structured returns are enough to decide whether to harden classification, improve packaging, change the offer or stop this direction.

### Scope

- Publish the verified code, one-command pilot and GitHub return form.
- Invite no more than ten initial reviewers from the existing technical audience.
- Count only reproducible receipts plus explicit human verdicts.
- Measure classification usefulness, second-run intent, seven-day return and concrete support level.
- Record anonymized aggregate outcomes in `data/ghost-audit-pilot.json` without publishing sensitive inputs.
- Use reviewer errors to add focused evaluation cases before changing the classifier.
- Decide the next engineering investment from the gate instead of assuming product-market fit.

### Non-goals

- No claim that GitHub unique cloners are identified humans or customers.
- No secrets, seed phrases, passwords or confidential text in audit inputs.
- No arbitrary filesystem, shell, Wallet, public-posting or paid-provider execution.
- No remote-attestation or factual-truth claim from the local integrity receipt.
- No investment, yield, guaranteed return or cash-redemption wording in the support offer.

### Verification gate

- `./tools/verify-v1.sh`
- valid GitHub issue form after push
- ten receipt/review records or an explicit end date with the actual shortfall
- aggregate pilot counters updated without sensitive content
- public copy limited to what the receipt and human-return data actually prove

## Product roadmap to v1.0

### Milestone 0 - Publish the current baseline

- [x] Read repository and CMS documentation.
- [x] Identify current state and pending local commits.
- [x] Add this completion plan.
- [x] Push current SolOS and CMS state.

Acceptance:

- GitHub has the latest Ghost classification/trace commit and the completion-plan commit.
- CMS/webapp has a visible completion status and executive summary.

### Milestone 1 - Ghost trace persistence

- [x] Persist trace outcomes locally.
- [x] Add accepted/rejected/corrected status.
- [x] Add expected-route examples for common SolOS request classes.
- [x] Show trace evaluation status in Agent/Ghost.
- [x] Document trace contract and evaluation seed.

Acceptance:

- A future Ghost request can be compared against stored examples before claiming a route is safe or ready.

### Milestone 2 - Tool and approval manifest

- [x] Define tool/capability manifest schema.
- [x] Classify capabilities by read/write/sensitive/network/wallet/public scopes.
- [x] Bind approval requirements to scope.
- [x] Show required approval reason in Agent/Ghost.

Acceptance:

- Ghost can explain why a request needs approval and which capability boundary is involved.

### Milestone 3 - First mediated action

- [x] Choose one low-risk mediated action.
- [x] Route it through runtime-mediated command/task state.
- [x] Require approval when appropriate.
- [x] Emit trace and result.

Preferred first action:

- Open a documented local app/module or safe URL from the Apps surface.

Acceptance:

- The user can see request, route, approval need, action result, and trace outcome without hidden execution.

### Milestone 3.5 - Ghost Resolution Loop

- [x] Make a resolution selectable and give it an explicit target outcome.
- [x] Persist ordered steps and legal transitions in a versioned Daemon-owned store.
- [x] Stop at approval before the first mediated capability.
- [x] Resolve only after `app.state.read` verifies the target state.
- [x] Retain approval, capability result and verification evidence after Daemon restart.
- [x] Expose the same journey in native and web Agent surfaces.

Acceptance:

- A reviewer can follow one objective from selection to a verified result without hidden execution or an unsupported capability claim.

### Milestone 3.6 - Ghost real-input audit pilot

- [x] Accept exact reviewer input instead of only a seeded objective.
- [x] Treat embedded instructions as inert data and expose deterministic scope/risk signals.
- [x] Stop at explicit approval before a real isolated Linux filesystem effect.
- [x] Verify read-back in a separate executable and bind input/artifact SHA-256 to a portable receipt.
- [x] Fail closed on artifact tampering.
- [x] Persist the audit and receipt across Daemon restart.
- [x] Expose the flow in the native Agent surface and a one-command clone pilot.
- [x] Define a ten-reviewer GitHub return and support-signal gate.

Acceptance:

- Ten independent clone users can run their own inputs, return machine receipts plus human verdicts, and give measurable evidence about usefulness, repeat intent and willingness to support continued work.

### Milestone 4 - Quota accounting

- [ ] Connect Ghost research usage to local quota decrementing.
- [ ] Preserve BYOK fallback.
- [ ] Add service-unavailable and quota-exhausted states.
- [x] Draft server-side quota/proxy endpoint shape.

Acceptance:

- SolOS can show who pays for a research call, what quota remains, and what happens when quota is unavailable.

### Milestone 5 - Wallet and Heart Pass hardening

- [x] Require signed holder/session proof for future sponsored quota.
- [x] Isolate wallet/pass verification status from UI-only assumptions.
- [x] Add clearer verification failure reasons.
- [ ] Keep non-financial public wording.

Acceptance:

- Heart Pass utility remains access/usage oriented, not financial-return oriented.

### Milestone 6 - App launcher and capability boundary

- [ ] Turn Apps from registry surface into launcher bridge.
- [ ] Map each app/module to declared capabilities.
- [ ] Feed launcher actions into trace and approval policy.
- [ ] Keep SolOS Pulso visible as planned until consent, credit ledger, and cost model are real.

Acceptance:

- Apps behave like modules in the operating layer, not just static cards.

### Milestone 7 - Demo appliance

- [ ] Build web shell assets for browser-kiosk validation.
- [ ] Package the demo Linux appliance path.
- [ ] Test ISO or VM boot path.
- [ ] Decide when native shell becomes primary demo session.
- [ ] Document smoke-test checklist.

Acceptance:

- A reviewer can boot or run a demo environment and understand SolOS without reading the whole repository first.

### Milestone 8 - Product polish and release discipline

- [ ] Stabilize theme tokens and scrolling.
- [ ] Fix content-heavy screen behavior.
- [x] Add release checklist.
- [ ] Add architecture decision records for major choices.
- [~] Tag a v1 demo candidate after the final clean verification and push.

Acceptance:

- SolOS v1.0 is demonstrable, documented, scoped, and honest about what is prototype, what is runtime-mediated, and what remains future work.

## Prototype board

### Prototype A - Trace persistence

Question: can Ghost preserve and compare route outcomes without pretending to learn more than it does?

Deliverable: local trace store, expectation examples, Agent/Ghost trace summary.

### Prototype B - Quota usage accounting

Question: can Ghost research spend be made visible before any sponsored backend exists?

Deliverable: local decrementing, exhausted state, BYOK fallback copy, runtime contract update.

### Prototype C - First mediated action

Question: can SolOS execute one low-risk action through request classification, approval policy, runtime mediation, and trace result?

Deliverable: approved open-app/open-URL action with visible status.

### Prototype D - Demo appliance smoke

Question: can SolOS be shown as a Linux-based operating layer in a repeatable VM/demo flow?

Deliverable: built shell assets, appliance context, VM smoke notes, release candidate checklist.

### Prototype E - Pulso Credits ledger

CMS/Supabase slice implemented on 2026-08-01:

- [x] Add a Founder supporter profile and permanent `founder-heart` badge contract.
- [x] Add idempotent campaign reward grants backed by `pulso_credit_ledger`.
- [x] Add authenticated balance/history surface at `/solos/pulso/recompensas`.
- [x] Add the first atomic redemption: 10 Pulso Credits for a reservation of 25 Ghost queries.
- [x] Apply and smoke-test the Founder Rewards migration in the production Supabase project.
- [x] Add an authenticated CMS claim code to each Ghost-query redemption.
- [x] Add a wallet-bound claim endpoint with idempotent reclaims for the same Polygon wallet.
- [x] Let the native Wallet claim the code only after Heart Pass ownership verification and add the entitlement to the visible Ghost quota.
- [x] Add a fail-closed Rust quota meter that persists local Ghost query consumption without charging on snapshot refresh.
- [ ] Connect actual provider-backed Ghost query execution to the Rust meter and remote usage sync; paid-provider execution still follows the existing BYOK/proxy boundary.

Question: can SolOS return social-signal value as internal utility credit without creating hidden cost or false financial promises?

Deliverable: credit ledger schema, caps, expiry, anti-fraud rules, Wallet-facing summary, and non-financial public wording.

## Decisions to discuss with Luiz

- Which demo story should define SolOS v1.0: Ghost trace, Heart Pass utility, app launcher, or Linux appliance boot?
- What is the first low-risk mediated action worth making real?
- What quota numbers should the first public Heart Pass experiment use?
- What should the first Pulso Credits redemption be: Ghost usage, upload allowance, access, discount, or creator tool?
- Should v1.0 prioritize native shell as the primary demo, or keep browser kiosk as the first externally repeatable path?
- What public promise should be avoided until the quota backend and approval router are real?

## Working rule

Every meaningful SolOS change should update:

1. SolOS repository documentation.
2. CMS/public or executive documentation when public framing, roadmap, or product status changes.
3. Devlog or memory when the decision affects future work.
