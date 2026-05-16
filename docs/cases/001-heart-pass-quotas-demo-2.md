# Case 001 — Heart Pass quotas, Ghost research, and SolOS demo 1.5/2.0

Status: first engineering case under the firm philosophy.

## 1. Problem framing

We need SolOS to support a paid/supporter pass model where a verified Heart Pass holder can use Ghost web research without every interaction feeling like manual infrastructure work.

At the same time, the SolOS philosophy requires legibility: the user should know what is being used, who pays, what the limits are, and what happens when a quota is exhausted.

## 2. Core tension

- **Convenience:** paid pass holders expect some included utility.
- **Ownership:** SolOS should not hide costs or silently burn a shared key.
- **Safety:** public clients must not expose Brave/OpenAI keys.
- **Adoption:** BYOK is honest but too hard as the only path for normal users.
- **Engineering:** the system must be segmented now before mockups and prototypes become a future maintenance problem.

## 3. Case segmentation

### Case A — BYOK-only supporter

The user verifies Heart Pass and brings their own Brave key.

Use when:
- early prototype;
- local demo;
- technically fluent supporter;
- no hosted backend yet.

Pros:
- immediate;
- honest billing boundary;
- no central provider key exposure.

Cons:
- high friction;
- weak paid-pass benefit perception.

### Case B — Paid pass with sponsored research quota

The user verifies Heart Pass and receives a monthly Ghost research allowance through a SolOS-controlled backend/proxy.

Use when:
- public supporter utility matters;
- we can host a quota service;
- we need admin supervision over costs.

Pros:
- real pass utility;
- clean UX;
- quotas remain legible;
- provider keys stay server-side.

Cons:
- requires backend;
- requires accounting/rate limiting;
- introduces operating cost.

### Case C — Hybrid fallback

Verified holder gets a sponsored monthly allowance first; after it is exhausted, Ghost offers BYOK continuation.

Recommendation: **Case C is the best target architecture.**

Why:
- gives the pass concrete value;
- preserves user sovereignty;
- limits firm exposure;
- aligns with SolOS: convenience without hiding cost.

## 4. Proposed solution architecture

### Local SolOS client

Responsibilities:
- verify/pass state visibility;
- wallet capture and Polygon verification status;
- display quota state;
- route Ghost research requests;
- fallback to BYOK when needed;
- never expose firm provider keys.

### SolOS quota service

Responsibilities:
- receive authenticated/verified pass requests;
- enforce monthly quota per wallet/pass;
- call Brave Search using a server-side key;
- return search results and quota metadata;
- log usage events for supervision.

### Data model draft

```json
{
  "walletAddress": "0x...",
  "pass": {
    "network": "polygon",
    "contract": "0x507783149b7abb6ce23414dd0c9742eb9f4549b4",
    "tokenId": "1",
    "standard": "ERC-1155",
    "status": "verified-holder"
  },
  "quota": {
    "period": "2026-05",
    "includedQueries": 100,
    "usedQueries": 0,
    "remainingQueries": 100,
    "fallback": "byok"
  }
}
```

## 5. Observability experience

The user should see:

- pass verification state;
- quota period;
- remaining included searches;
- whether Ghost is using sponsored quota or BYOK;
- failure state if quota service is unavailable;
- next action: verify pass, configure BYOK, wait for reset, or top up.

The admin/firm should see:

- total quota pool spend;
- usage by wallet/pass/month;
- error rates;
- suspicious spikes;
- provider failures;
- conversion path from pass verification to Ghost usage.

## 6. Demo 1.5 / 2.0 plan

### Demo 1.5 — disciplined prototype cleanup

Goals:
- backup current mock/prototype surfaces;
- inventory mockups and generated build artifacts;
- tag files as `runtime`, `ui`, `demo`, `mock`, `generated`, or `archive-candidate`;
- keep the current native shell buildable;
- add explicit status surfaces for Heart Pass and quota design.

Deliverables:
- mockup inventory report;
- backup/archive directory for removed mock assets;
- scripts to inventory and request cleanup;
- documented decision on what remains in demo.

### Demo 2.0 — Linux-base pushable demo

Goals:
- produce a cleaner Linux demo base;
- ship native shell with Heart Pass/Quota surfaces;
- include provisioning scripts;
- keep runtime snapshot generation reproducible;
- separate generated build artifacts from source.

Deliverables:
- filtered shell source;
- reproducible provision script;
- demo build notes;
- smoke-test checklist;
- push-ready branch/tag plan.

## 7. Mockup filtering principle

Mockups are useful until they become architectural debt.

Rule:
- mockups may remain only if they demonstrate a current product question;
- old mockups should be archived, not deleted blindly;
- generated artifacts should be excluded from source review unless intentionally tracked;
- every visible demo screen should map to a runtime contract or documented future seam.

## 8. First actionable next steps

1. Create a mockup/prototype inventory script.
2. Create a cleanup request/report script for demo files.
3. Add quota state to the Heart Pass runtime contract as `planned` before implementing backend.
4. Draft an ADR for Case C: sponsored quota + BYOK fallback.
5. Decide quota numbers for first public experiment.

## 9. Philosophical fit

The paid Heart Pass is not a shortcut around ownership. It is a supervised convenience layer.

SolOS should make the hidden economy visible:

- who owns access;
- who pays for search;
- what the quota is;
- when the limit is reached;
- how the user can continue.

That is the bridge between convenience and sovereignty.
