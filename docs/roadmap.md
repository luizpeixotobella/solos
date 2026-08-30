# SolOS Roadmap

Updated: 2026-08-29

This roadmap is a compact status map. The acceptance criteria and release gates live in `docs/product-completion-plan.md`.

## 1. Foundation and product language

- [x] Native Qt/QML shell with Home, Agent/Ghost, Wallet and Apps.
- [x] Linux base -> Rust runtime intermediary -> SolOS operating layer architecture.
- [x] Reusable QML theme tokens and bounded scrolling for content-heavy screens.
- [x] Product thesis, architecture, roadmap, devlog and release checklists.
- [x] Coherent-outside/modular-inside ADR and domain extraction rule.

## 2. Runtime and mediation

- [x] Host-derived facts and versioned compatibility snapshot.
- [x] Owner-only persistent Daemon over a Unix socket.
- [x] Typed capabilities, default-deny approvals and one safe mediated action.
- [x] Durable, atomic stores for Ghost resolutions and real-input audits.
- [x] Bounded event/evidence ledger that survives Daemon restarts.
- [x] Cursor-based minimized event export with no local payload disclosure.
- [~] Continue extracting domain controllers/services from the broad Qt controller.
- [~] Continue replacing snapshot-only state with narrow Daemon APIs without breaking compatibility.

## 3. Ghost system intelligence

- [x] Doctrine, request classification, route explanation and action trace.
- [x] Selected -> planned -> approval-bound -> resolved journey with retained evidence.
- [x] Ghost Audit Challenge, portable verifier and ten-reviewer evaluation contract.
- [x] Scoped memory/revocation and provider-neutral retrieval contracts.
- [x] Durable operational evidence exported to the private CMS Ghost Brain Monitor through HMAC authentication.
- [~] Capture explicit human accepted/rejected/corrected feedback as evaluation data separate from operational logs.
- [ ] Complete the ten-reviewer pilot and publish an evidence-based usefulness/return report.
- [ ] Connect a provider-backed research executor only after credentials, cost ownership and signed server proof exist.
- [ ] Add multilingual intent/tone evaluation with source-language citations; do not reduce this to UI translation.

## 4. Wallet, quota and Heart Pass

- [x] Heart Pass ownership/session proof visible in runtime and Wallet.
- [x] Local fail-closed quota metering, exhaustion state and BYOK fallback.
- [x] Provider-neutral signed quota/proxy contract.
- [x] Founder reward grant, redemption, atomic Wallet binding and Pulso Credits ledger.
- [ ] Authorize and meter a real provider request only after the external secret/cost gate is approved.
- [ ] Expand Wallet beyond the current pass/quota account path when a concrete second account capability exists.

## 5. Apps and Pulso

- [x] Registry with stable IDs, status, capability and launch target.
- [x] Exact-allowlist native launcher; each launch is recorded through the Daemon.
- [x] Controlled Pulso web adapter in the SolOS Apps surface.
- [x] Pulso Alpha 0.2 production Value Loop: adult consent, moderation, export/delete, semantic reactions, comments, profiles, reposts, private images, disclosed sponsored agents, Ghost topics and credits guardrails.
- [ ] Create a native Pulso surface only when it adds a real system capability beyond the web adapter.
- [ ] Add another app integration only through the same declared-capability and evidence boundary.

## 6. Appliance and release

- [x] Browser-kiosk and native shell packaging paths.
- [x] Web, Rust, Ghost resolution/audit and native smoke gates in `tools/verify-v1.sh`.
- [x] Daemon installer and optional five-minute CMS evidence-sync timer.
- [x] Dependency audit with zero known npm vulnerabilities at this release pass.
- [ ] Build and boot the demo ISO in a VM on a host with `live-build`, `xorriso` and QEMU.
- [ ] Tag the public demo candidate after VM boot proof and ten-reviewer evidence are attached.

## External gates, not hidden code debt

- Human Ghost reviewer responses.
- Provider credentials, paid-usage ownership and signed production proof.
- Stripe account-side links/secrets and fiscal/legal review where applicable.
- A VM-capable build host for ISO boot evidence.

These gates must remain visible and fail closed. They are not permission to fake completion, embed shared secrets or claim autonomous learning.
