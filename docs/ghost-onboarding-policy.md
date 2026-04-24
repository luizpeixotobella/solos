# Ghost onboarding policy

## New rule

Ghost web research must never default to the developer's Brave API key in public or shared SolOS builds.

Each SolOS user should acquire and configure their own Brave Search API key.

## Why this rule exists

If SolOS ships with a shared developer key:

- usage can spike unpredictably
- billing and quota become unsafe
- one user's activity can degrade another user's experience
- the system stops being a credible ownership-oriented operating layer

That violates the basic design posture of SolOS.

Ghost should feel native and useful, but it should not hide account ownership or cost boundaries.

## Required onboarding flow

1. Ghost detects that no user key is configured.
2. Ghost surfaces the Brave key onboarding URL inside the Agent surface.
3. The user is sent to Brave's key page to subscribe or pay on their own account.
4. The user returns to SolOS and pastes their Brave key into the Agent onboarding form.
5. SolOS validates the key against Brave before persisting it.
6. Only valid keys are saved into `solos/config/ghost.json`.
7. Ghost refreshes its runtime state and enables web-grounded research.

## Current implementation status

Implemented now:

- repository-local Ghost config at `solos/config/ghost.json`
- onboarding URL surfaced in the native Agent screen
- repo-local key persistence from the shell UI
- validation-before-save flow using Brave's live API
- clear-key flow to return the repo to a safe bootstrap state
- runtime snapshot fields exposing onboarding state

Not yet implemented:

- richer key-format prevalidation before remote validation
- callback or deep-link return flow from Brave into SolOS
- encrypted at-rest secret handling for user keys
- multi-user profile isolation for shared machines

## Working product rule

Every Ghost capability that depends on paid external infrastructure should make ownership, approval, and billing boundaries legible.

SolOS should not smuggle third-party costs behind a magical assistant surface.

## Immediate next modules after onboarding

1. `ghost-intents`
   - classify user intent
   - decide when to answer, research, suggest, or request approval

2. `ghost-actions`
   - route approved intents into explicit SolOS actions
   - keep execution and scope legible

3. `ghost-memory`
   - preserve useful local context without collapsing ownership boundaries
