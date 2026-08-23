# SolOS v1.0 RC2 — Ghost Audit Pilot Release Gate

Date: 2026-08-23

## Release definition

SolOS v1.0 RC2 keeps the RC1 Linux/runtime/operating-layer contracts and adds the first real-input Ghost audit that a cloned repository user can run without trusting a marketing post or the native interface.

## Shipped in RC2

- Daemon-owned schema `solos.ghost.audits.v1` for exact input, classification, approval, result, verification and evidence.
- Owner-only real Linux artifact schema `solos.ghost.audit.artifact.v1`.
- Portable verifier receipt schema `solos.ghost.audit.receipt.v1`.
- Transparent deterministic scope/risk classification with `embeddedInputExecution=false`.
- Narrow, approval-bound `ghost.audit.proof.write`; arbitrary paths and embedded commands are never executed.
- Separate `ghost-audit-verify` executable/process for artifact read-back and hash checks.
- Daemon binding between verifier output and the input/artifact SHA-256 retained at write time.
- Tamper failure, denial safety, atomic files and Daemon restart persistence.
- Native Agent/Ghost audit surface.
- One-command clone pilot: `tools/ghost-audit-pilot.sh`.
- Ten-reviewer GitHub return form, acquisition baseline and stop/go validation contract.

## Verification

```bash
./tools/verify-v1.sh
```

The gate checks formatting, Rust unit tests, Daemon RPCs, hostile input inertness, denial, real artifact creation, external verifier receipt, tamper failure, restart persistence, snapshot invariants, web build/smoke, native build/QML smoke and appliance scripts.

## Honest boundary

- This is an integrity and safe-routing audit around submitted text, not factual web verification.
- The deterministic classifier is open to human disagreement; the ten-reviewer pilot measures that semantic usefulness.
- The local receipt is reproducible but is not signed remote attestation or a production security certification.
- The only real effect is an isolated owner-only proof artifact. Ghost still does not execute arbitrary input, wallet actions, public posting or paid provider calls.
- Exact input is stored locally. Reviewers must not submit secrets or confidential content.
- GitHub unique cloners are an aggregate acquisition signal, not identified people or customers.

## Candidate gate

Tag `v1.0.0-rc2` only after:

- `./tools/verify-v1.sh` passes from a clean working tree candidate;
- the pilot script produces a passing receipt for a real input;
- a deliberate artifact modification fails closed;
- the GitHub issue form is accepted after push;
- public copy matches `docs/ghost-audit-pilot.md` and does not reuse the overclaim that Ghost verifies arbitrary truth or autonomously executes input.
