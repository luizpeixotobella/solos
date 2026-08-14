# SolOS Shell Native

Native Qt/QML operating-layer shell for SolOS.

## Intent

This directory contains the first-class native shell track. The web shell remains a portable demo/reference surface; the native shell consumes runtime-mediated state and requests without becoming the persistent service layer.

The web shell remains the UX reference.
The native shell becomes the long-term runtime candidate.

## Stack

- Qt 6
- QML
- C++
- CMake

## Current scope

The current shell provides:

- a minimal Qt/QML application entrypoint
- a C++ `AppController` exposed to QML
- sidebar navigation for Home / Agent / Wallet / Apps
- top status bar backed by structured runtime state
- model-backed Home, Agent/Ghost, Wallet and Apps surfaces
- Heart Pass verification/quota and Pulso reward synchronization flows
- Ghost readiness, classification, trace, citations and approvals surfaces
- direct `snapshot.get` reads from the owner-scoped SolOS Daemon
- automatic retry and safe file fallback when the Daemon is unavailable

## Runtime boundary

The shell now includes a first integration seam:

- `src/runtime_snapshot.json` acts as the RC1 compatibility/fallback read model
- `src/runtimebridge.*` parses structured runtime state into shell models
- the bridge first queries the local Daemon socket and falls back to the file on timeout or invalid data
- persistent/systemic behavior belongs in the Daemon; QML remains presentation and request UX

That means the shell can become more real without requiring a full rewrite.

## Build intent

Expected build flow on a machine with Qt 6 + CMake installed:

```bash
cmake -S app/shell-native -B app/shell-native/build
cmake --build app/shell-native/build
./tools/smoke-native-shell.sh
```

## Notes

- The RC1 is demonstrable, not a production Wallet, autonomous agent or arbitrary host-command executor.
- A successful build is not sufficient: the repository verification gate launches the Daemon and shell together and rejects QML runtime errors.
- The broad `AppController` is the next internal modularity target; it should split by natural domain ownership without fragmenting the shell experience.
