# SolOS Daemon v1

The SolOS Daemon is the persistent process boundary of the runtime intermediary. Linux still owns boot, scheduling, processes and devices; the Daemon owns normalized runtime state, safe mediation, local events and stable contracts consumed by SolOS surfaces.

## Natural ownership rule

- Persistent, systemic, background and Linux-mediated behavior belongs in the Daemon.
- Home, Ghost, Wallet, Apps and Pulso retain their domain-specific presentation and user flows.
- The shell requests actions and renders state; it does not become a second service layer.
- Linux primitives are reused rather than duplicated.

## Initial protocol

`solos-daemon` listens on an owner-only Unix socket and accepts newline-delimited JSON. The v1 surface is deliberately small:

- `health.get`: daemon health, uptime and snapshot availability.
- `snapshot.get`: the existing runtime snapshot, preserving RC1 compatibility.
- `events.health`: schema, retained count, sequence and owner-local store health.
- `events.list`: cursor-based reads from the bounded durable ledger.
- `events.export`: minimized CMS envelopes that deliberately omit every local payload.
- `event.publish`: restricted `shell.*`, `ghost.*`, `pulso.*`, `wallet.*` and `apps.*` local events.
- `ghost.resolutions.get`: read the Daemon-owned Ghost resolution store.
- `ghost.resolution.select`: select a ready objective.
- `ghost.resolution.start`: build its bounded plan and open the approval boundary.
- `ghost.resolution.decide`: record approval or denial, then resolve or stop without a side effect.
- `ghost.resolutions.reset`: restore the RC1 beta seed.
- `ghost.audits.get`: read the Daemon-owned real-input audit history.
- `ghost.audit.prepare`: preserve and classify exact user input without executing it.
- `ghost.audit.decide`: deny safely or approve only the isolated `ghost.audit.proof.write` capability.
- `ghost.audit.verify`: invoke the separate `ghost-audit-verify` process, bind its receipt to the retained hashes and fail closed on mismatch.

Example request:

```json
{"id":"health-1","method":"health.get"}
```

The socket directory is mode `0700` and the socket is mode `0600`. The protocol does not expose arbitrary shell execution, wallet actions or network calls. Resolution, audit and event-ledger persistence use versioned Daemon-owned stores committed atomically. `SOLOS_EVENT_STORE` can override the XDG/user-state default.

## Ghost ecosystem bridge

`tools/sync-ghost-brain.mjs` reads only `events.export`, signs the exact request body with HMAC-SHA256 and advances an owner-local sequence cursor only after the CMS accepts the batch. The exported envelope contains event identity, kind, timestamp, lifecycle stage and sequence; it never copies the local `data` payload. The CMS endpoint accepts only `source=solos_daemon`, rejects personal-data flags and unsafe evidence keys, checks a five-minute signature window and deduplicates by event key.

The optional `solos-ghost-sync.timer` runs every five minutes. Store the shared secret only in the local protected environment file (or point `SOLOS_GHOST_CMS_SECRET_FILE` at a mode-0600 file) and the CMS deployment secret manager. `appliance/demo-linux-v1/bin/install-user-runtime.sh` installs and enables the owner-local Daemon/timer without root; the appliance installer remains available for `/opt`. The CMS remains an observability/review surface; mediation and local evidence ownership stay in the Daemon.

Ghost audit bundles are also owner-only (`0700` directory, `0600` JSON files). Set `SOLOS_GHOST_AUDIT_STORE`, `SOLOS_GHOST_AUDIT_DIR`, and `SOLOS_GHOST_AUDIT_VERIFIER` to isolate a pilot run. The submitted input is deliberately retained verbatim in the local artifact, so secrets must never be used as audit inputs.

`snapshot.get` injects the latest `ghost.resolutionLoop` read model instead of serving stale generated state. Set `SOLOS_GHOST_RESOLUTION_STORE` to override the state path; otherwise the Daemon uses the XDG/user local state directory.

## Run locally

```bash
cd app/runtime-core
SOLOS_DAEMON_SOCKET=/tmp/solos-daemon.sock \
SOLOS_RUNTIME_SNAPSHOT=../shell-native/src/runtime_snapshot.json \
cargo run --bin solos-daemon
```

The appliance unit is `appliance/demo-linux-v1/config/systemd/solos-daemon.service`. The native runtime bridge now reads `snapshot.get` from the Daemon, retries on its normal refresh cycle and falls back to the snapshot file when the service is unavailable or returns invalid data.

Install a release build on a demo host with:

```bash
sudo SOLOS_REPO=/path/to/solos appliance/demo-linux-v1/bin/install-daemon-build.sh
systemctl --user enable --now solos-daemon.service
```
