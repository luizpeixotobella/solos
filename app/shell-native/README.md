# SolOS Shell Native

Initial native shell scaffold for the Linux-aligned SolOS migration path.

## Intent

This directory starts the transition from the current web shell (`app/shell`) toward a native runtime.

The web shell remains the UX reference.
The native shell becomes the long-term runtime candidate.

## Stack

- Qt 6
- QML
- C++
- CMake

## Current scope

This scaffold currently provides:

- a minimal Qt/QML application entrypoint
- a C++ `AppController` exposed to QML
- sidebar navigation for Home / Agent / Wallet / Apps
- top status bar with mocked session/system/wallet data
- main workspace panels for each primary surface
- basic structure for future native state models and Linux integration
- a runtime snapshot bridge that allows structured shell state to be loaded from a machine-readable contract

## Runtime boundary

The shell now includes a first integration seam:

- `src/runtime_snapshot.json` acts as the current fixture contract
- `src/runtimebridge.*` parses structured runtime state into shell models
- this boundary is designed so a Rust subsystem can replace the static fixture incrementally

That means the shell can become more real without requiring a full rewrite.

## Build intent

Expected build flow on a machine with Qt 6 + CMake installed:

```bash
cmake -S app/shell-native -B app/shell-native/build
cmake --build app/shell-native/build
```

## Notes

- This is not wired to real agent, wallet, or Linux services yet.
- Mock data is deliberate at this stage.
- The current goal is to establish native structure, not finish the shell.
