# Case 004 - AI Ghost Classification and Action Trace

Date: 2026-07-08

## Purpose

Turn the archived W3Schools AI/Machine Learning material into the next small Ghost implementation slice:

- classify user requests before routing
- expose the action trace behind a Ghost response or action
- explain why Ghost selected a route
- keep quota, approval, and external-action boundaries visible

This is not a neural training system yet. It is a deterministic, inspectable router inspired by the AI framing that useful data plus desired results can shape algorithmic behavior.

## Runtime contract

`runtime-core` now emits three additional Ghost objects:

- `ghost.requestClassifier`
- `ghost.actionTrace`
- `ghost.routeExplanation`

The request classifier carries classes such as:

- research request
- system action request
- wallet/pass request
- documentation task
- content/publishing task
- risky external action
- memory/update request
- unclear request

Each class includes:

- safety level
- required tools
- approval needs
- quota cost
- selected route

## Product behavior

The current example request is:

```text
Turn the archived W3Schools AI material into the next AI Ghost implementation slice.
```

Ghost routes it as:

```text
documentation task + system action request
```

The selected route is local implementation from archived evidence, because the W3Schools archive already exists and the work does not need fresh web research, public posting, wallet signing, or paid provider calls.

## UI surfaces

The native Agent/Ghost screen now shows:

- Ghost request classifier
- Ghost action trace
- route explanation

The web/mock Agent screen mirrors the same concept so the prototype and native shell keep the same product story.

## Next step

Persist trace outcomes and accepted/rejected examples. The next slice should let Ghost compare future requests against expected classes and routes before the UI claims the action is safe or ready.

This next slice is now tracked as **Ghost Trace Persistence and Evaluation Seed** in `docs/product-completion-plan.md`.
