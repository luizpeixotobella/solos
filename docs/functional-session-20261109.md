# Functional SolOS: staged recovery, 2026-11-09

Luiz requested working runtime/Explorer/Wallet and automatic setup, explicitly allowing small stages to fit resources and token budget.

## Stage 1 — installed session plumbing

Identified causes: the image omitted the runtime snapshot producer executable, the daemon used a relative state-file path without a publisher, the native refresh tried `cargo run` against build-tree paths, and the Explorer was only an optional launcher while the old shell owned the session. Native snapshot fallback could mask missing integration. Qt resource-prefix defaults also varied between Qt versions.

Candidate fixes:

- Package `solos-runtime-core`; generate host state before daemon start and refresh through a user timer. Validate JSON and atomically replace a 0600 file inside a 0700 user-runtime directory; preserve the previous state on generation failure.
- Automatic `solos-setup`, no sudo, no manual file edits. Verify binaries/units, enable the supplied user services, inspect the existing folder association and retain its previous value before selecting Explorer. `--check` is read-only.
- Start Explorer as the fullscreen session, guard against duplicate session loops, support directory arguments and default folder handling. Keep the native shell accessible as an application.
- Native refresh uses the installed executable and a writable user path. Read from daemon by default; file fallback is explicit opt-in. No embedded demo state is promoted as daemon success.
- Explorer's status requires a daemon health reply with a state file; it continues checking instead of staying green forever after the first socket connection.
- Explicit QML resource/import roots and Qt6.4 compatibility; packaging gate tests copied GUI binaries outside the build tree.

## Verification commands

Build the Rust binaries with the locked Cargo file, build both Qt applications, then:

```sh
python3 appliance/ubuntu-ji/tests/functional-runtime.py PATH_TO_RUST_BINARIES PATH_TO_QT_BUILD/runtime-daemon-smoke
ctest --test-dir PATH_TO_QT_BUILD --output-on-failure
```

The Python test uses the **actual compiled generator and daemon**, private temporary state, a real Unix socket, and the native Qt bridge if supplied. It checks host identity, schema/mode, file/socket permissions, and failed-refresh preservation. The GUI installed-layout smoke must exit124 after six seconds with no QML import/type failure.

The first installed-layout attempt caught a missing QML resource path after lowering the minimum Qt version. Explicit `RESOURCE_PREFIX /qt/qml` corrects the policy-dependent prefix; retain this check across future releases.

## Not completed by Stage 1

This change does **not** make all snapshot domains production-backed. Wallet, quota/rewards, approvals and some Ghost panels still contain prototype assumptions and require independent integration. A real host snapshot is not evidence of a real balance, valid entitlement, operational inference or completed action.

## Next bounded stages

1. Package a NEW ISO from this candidate, preserve the previous ISO/VM, verify checksum and boot automatic Explorer plus real runtime in a separate VM. Do not disturb an active owner's VM, install to physical disks, reboot the host or expose network services. Existing WSL build caches may be reused only after inspecting their state; do not duplicate builds.
2. Replace wallet/prototype presentation with an explicit disconnected/connected state, an in-app setup flow and real read-only chain/account queries. Keep the Safe lab optional/outside the default image. No mainnet transactions, new paid services or seed/key custody without a specific owner decision.
3. Configure self-hosted Ghost through the UI and verify a real response. API credentials remain private; do not bake them into an ISO. Validate each remaining panel independently and label unavailable functions plainly.

Build-tree/offscreen tests are not boot proof. A source push is not an installed update. Promote only with the corresponding evidence.
