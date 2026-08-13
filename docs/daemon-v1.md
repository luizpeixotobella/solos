# SolOS Daemon v1

The SolOS Daemon is the persistent process boundary of the runtime intermediary. Linux still owns boot, scheduling, processes and devices; the Daemon owns normalized runtime state, safe mediation, local events and stable contracts consumed by SolOS surfaces.

## Natural ownership rule

- Persistent, systemic, background and Linux-mediated behavior belongs in the Daemon.
- Home, Ghost, Wallet, Apps and Pulso retain their domain-specific presentation and user flows.
- The shell requests actions and renders state; it does not become a second service layer.
- Linux primitives are reused rather than duplicated.

## Initial protocol

`solos-daemon` listens on an owner-only Unix socket and accepts newline-delimited JSON. The v1 read surface is deliberately small:

- `health.get`: daemon health, uptime and snapshot availability.
- `snapshot.get`: the existing runtime snapshot, preserving RC1 compatibility.
- `events.list`: bounded in-memory event history.
- `event.publish`: restricted `shell.*` and `ghost.*` local events.

Example request:

```json
{"id":"health-1","method":"health.get"}
```

The socket directory is mode `0700` and the socket is mode `0600`. The protocol does not expose arbitrary shell execution, filesystem writes, wallet actions or network calls.

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
