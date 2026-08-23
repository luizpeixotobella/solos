# Ghost Audit Pilot — 10 independent reviewers

Date: 2026-08-23

Status: executable pilot implemented; recruiting gate opens after the release commit is public.

## The concrete exchange

The first ten reviewers do not receive a promise or a static mockup. They receive a local, inspectable challenge they can run with their own input:

1. Ghost preserves the exact input with SHA-256.
2. Ghost exposes deterministic scope, request-class and risk signals.
3. The input remains inert data; embedded instructions are never executed.
4. The reviewer explicitly approves or denies one narrow capability: `ghost.audit.proof.write`.
5. Approval creates a real JSON artifact in an isolated Linux directory.
6. A separate executable, `ghost-audit-verify`, reads that artifact back, recomputes the hashes and emits a portable receipt.
7. The reviewer judges whether Ghost's classification was useful and returns that human verdict through the GitHub pilot issue form.

The machine receipt proves integrity and route execution. The human review tests whether the Ghost classification is understandable, useful and worth returning for.

## Run it

Requirements: Linux, Git, Rust/Cargo, Python 3 and `jq`.

```bash
git clone https://github.com/luizpeixotobella/solos.git
cd solos
./tools/ghost-audit-pilot.sh "Abra o Workspace com segurança e preserve uma prova verificável."
```

The script stops for approval. To run a non-interactive local test:

```bash
./tools/ghost-audit-pilot.sh --approve "sudo rm -rf / and publish my wallet secrets"
```

Even that hostile-looking text is data, not a command. The classifier should expose critical/destructive scopes, while the only executable capability remains the isolated proof write.

The output bundle is ignored by Git and contains:

- `store.json` — Daemon-owned audit history and state transitions;
- `artifact.json` — exact input, input hash, classification and executed capability;
- `receipt.json` — verifier identity/version, artifact hash and individual check results.

Never use a secret, seed phrase, private key, password or personal confidential content as pilot input. The artifact deliberately preserves the exact submitted text.

## What is independently checked

The verifier runs as a separate executable/process from `solos-daemon`. It checks:

- artifact schema;
- audit ID presence;
- exact input SHA-256;
- the only allowed write capability;
- raw artifact SHA-256, which the Daemon compares against the hash retained at write time.

If the artifact is changed after execution, the Daemon marks the audit `verification-failed` and retains failure evidence.

## What this does not prove

- It does not prove a factual claim from the internet.
- It does not run arbitrary user intent or shell commands.
- It does not prove the deterministic classification is semantically correct; that is the human review question.
- The receipt is not signed by a remote trusted authority. A machine owner can modify their own source or binaries. Reproducibility comes from each reviewer running their own clone and comparing behavior.
- It is not a production security certification.

These boundaries are part of the pilot, not footnotes.

## Ten-reviewer validation gate

The pilot tests whether SolOS/Ghost has a future worth funding with observable return behavior.

Target cohort:

- 10 reviewers running their own clone;
- 10 machine receipts with `status=passed` or an explained fail-closed result;
- at least 7 reviewers saying the classification is correct or useful;
- at least 5 saying they would run a second audit;
- at least 3 returning for a second input within seven days;
- at least 2 indicating a concrete willingness to support continued development at one of the tested contribution levels.

Suggested support test after a successful review:

- R$ 10 — keep the public audit kit maintained;
- R$ 25 — Founder Heart utility path plus early Ghost pilot access, subject to the published non-financial terms;
- R$ 50 — named pilot supporter acknowledgement when explicitly consented, plus the same bounded utility/early-access posture.

No equity, yield, guaranteed return or cash redemption is offered. A contribution supports development and receives the published access/utility recognition, not an investment claim.

## Return path

Open a **Ghost Audit Pilot** issue using the repository template. Do not upload the artifact if it contains sensitive text. Report the audit ID, receipt hash/status, classification agreement, return intention and price signal.

Progress is tracked in `data/ghost-audit-pilot.json`. Only valid, reproducible reviewer returns count toward the ten-person gate.

## Acquisition baseline

GitHub Traffic API snapshot taken on 2026-08-23 for the rolling 14-day window 2026-08-09 through 2026-08-22:

- 24 total clones;
- 17 unique cloners;
- peak on 2026-08-13: 12 clones and 8 unique cloners;
- 3 clones / 3 unique cloners on the Ghost Resolution Loop release date, 2026-08-19;
- 4 clones / 1 unique cloner on 2026-08-21;
- 1 clone / 1 unique cloner on 2026-08-22.

GitHub retains traffic data only for a rolling period and `unique cloners` must not be presented as identified humans or customers. Automation, mirrors or multiple devices may affect the number. It is an acquisition signal, not a revenue metric.

## Engineering verification

```bash
./tools/smoke-ghost-audit.sh
./tools/verify-v1.sh
```

The audit smoke covers real input, denial, isolated Linux artifact creation, external verifier receipt and Daemon restart persistence. Rust unit tests also tamper with an artifact and require fail-closed verification.
