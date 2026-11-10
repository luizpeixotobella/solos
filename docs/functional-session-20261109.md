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

## Noble QML packaging correction (2026-11-09)

The first Stage2 image build compiled both GUIs but failed the installed-layout
gate: `QtQml.WorkerScript` was missing in the Noble chroot. Qt development
packages do not guarantee deployment of every QML module. The appliance now
explicitly installs QtQml, Models, WorkerScript and QtQuick Templates alongside
the existing QtQuick modules, and verifies their installed package state before
accepting the ISO. Retain both installed-layout smoke gates; a successful compile
is not sufficient. This dependency correction alone is not ISO or guest proof.

Noble live-build 3.x resume caution: when `cache/bootstrap` exists but
`.build/bootstrap_cache.restore` does not, the restore helper replaces the chroot.
Existing `.build/chroot_*` stamps are then stale and must not skip dependency,
source-overlay or hook stages. Inspect this state before resuming; preserve the
stamps for diagnostics and rerun all affected stages. A smoke pass before a
bootstrap restore does not validate the final packaged rootfs.

## Guest verification and stream-framing correction (2026-11-10)

The dependency-corrected ISO (SHA256
`0fe9a8b954033dc3c32de5024b5b196466289dea76008925be1421dfa1fc57e6`)
booted in a separate VirtualBox BIOS VM, with only its read-only ISO attached,
6 GiB RAM, two CPUs, no network, disks, clipboard or host shares. The earlier
owner VM was left running unchanged. After selecting the live entry, Explorer
started automatically fullscreen. Guest terminal checks confirmed the real
Ubuntu/hostname snapshot, active daemon and refresh timer, 0700 runtime directory,
0600 snapshot/socket, and `solos-explorer.desktop` as the folder association.
Opening `/home/solos` through `xdg-open` displayed the actual guest directories.
The new test VM was saved, with `VMState="saved"` verified.

The second Explorer window exposed an intermittent disconnected indicator.
Inspection found health JSON was parsed after the first available bytes rather
than after its newline delimiter. A real-socket fragmented-response regression
failed against the old algorithm and passes with the corrected reader. The
reader now has one total deadline and a 64 KiB response limit; incomplete,
malformed, oversized, negative and disconnected replies remain unhealthy.

Run `ctest --test-dir <explorer-build> --output-on-failure` for nine socket
scenarios in `runtime-health-framing`. This source correction is **not yet in
the tested ISO**; boot/autostart evidence does not prove it is installed. Wallet
and Ghost setup remain separate stages. Consolidate their verified source
changes before another expensive ISO rebuild, then repeat the guest gates.

## Stage 3 — public-address Wallet (2026-11-10)

Preflight reused the native Qt Network stack and the existing Heart Pass
ERC-1155 read path rather than adding a wallet SDK, signing connector, hosted
service or dependency. Shared `ChainRpc` validation now also checks the existing
Heart Pass response envelope; the existing ownership/ABI helpers remain intact.

The native Wallet and top bar no longer invent a Solana account or SOL/USDC
funds. The auxiliary web shell also starts disconnected with no assets/balances,
not a green connected badge. Native Wallet provides explicit save/clear/read
controls for a **public Polygon address** and an owner-selected RPC endpoint.
Configuration is atomic, private (`$XDG_CONFIG_HOME/solos/wallet.json`, file 0600,
directory 0700), and reloads without initiating network activity. HTTPS is
required except for loopback HTTP; credential/query/fragment URLs are rejected.
No address or endpoint is preconfigured as the owner's identity.

Reads are asynchronous: `eth_chainId` must return Polygon `0x89` before
`eth_getBalance`. Exact uint256 native quantities are rendered as POL without
floating-point rounding. JSON-RPC version/id/error, HTTP failures, malformed or
oversized responses, a total five-second per-request deadline, and canonical
quantity validation fail closed. A refresh/error clears old funds; an observation
expires after five minutes. Late replies cannot repopulate a cleared/reconfigured
address. No automatic balance query occurs at startup or save.

The UI explicitly distinguishes public observation from address control, NFT
entitlement and inference availability. Existing Heart Pass verification remains
a separate legacy read path, not a signing session. Local prototype quota totals
and claim actions are no longer presented as usable funds/allowance in Wallet.
The old Heart Pass repository-local persistence has **not** been migrated by this
bounded stage; it is separate from the new writable observer configuration.
No private keys/seeds, token balances, transactions, Safe lab, or sponsored
inference are implemented or asserted.

Verification:

```sh
cmake -S app/shell-native -B /tmp/solos-wallet-build -DBUILD_TESTING=ON
cmake --build /tmp/solos-wallet-build -j2
ctest --test-dir /tmp/solos-wallet-build --output-on-failure
(cd app/shell && npm ci --ignore-scripts && npm run build)
```

`wallet-local-rpc` runs an in-process loopback HTTP server with synthetic public
fixtures only. It covers invalid/zero addresses and unsafe endpoints, save/reload
and file permissions, Polygon chain ID, exact native balance, ten failure modes
(wrong chain/id/version, RPC error, malformed/noncanonical/overflow/oversized
responses, HTTP error and timeout), observed-balance expiry using an injected
clock, clearing old funds on errors, and clear during an in-flight request. It
also checks zero, one wei and maximum uint256 formatting. Both CTest targets and
the TypeScript/Vite build passed. A copied-binary isolated offscreen QML smoke is
recorded in the workspace stage report; it is not interactive guest evidence.

**Source-only:** no ISO rebuild, VM change, host config change, main promotion,
real owner's address query or live chain proof. The old tested image still lacks
both this Wallet stage and the Explorer framing fix. Ghost endpoint/key/model
setup is next, followed by one consolidated packaging/guest gate.
