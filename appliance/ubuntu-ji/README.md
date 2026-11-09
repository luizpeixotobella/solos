# SolOS Ubuntu JI image

This directory builds a bootable **Ubuntu 24.04 (Noble) live ISO** with:

- the native SolOS Qt/QML shell;
- the Rust SolOS runtime and Ghost verifier;
- GNOME Terminal and a normal Ubuntu desktop beneath SolOS;
- a fullscreen native kiosk session with retry-on-crash startup;
- a maximized SolOS Explorer with file browsing, installed-app launchers, and automatic runtime startup;
- a branded SVG wallpaper (a custom animated SolOS splash is not implemented);
- an Ubuntu desktop installer launcher available from the live session;
- the SolOS source tree in `/opt/solos-src` for local development;
- `solos-update`, an approval-bound update helper with verify/apply/rollback stages.

It is a test/development appliance, not yet a replacement for a general-purpose production OS. Linux remains the base; SolOS is the operating and intelligence layer above it.

## Build

Build on Ubuntu/Debian with at least 25 GB free disk and 8 GB RAM:

```bash
sudo apt-get install live-build debootstrap dctrl-tools genisoimage grub-pc-bin grub-efi-amd64-bin syslinux-utils xorriso squashfs-tools rsync fdisk dosfstools mtools ubiquity ubiquity-frontend-gtk
sudo ./build-iso.sh --fresh
```

The result is written to `out/solos-ubuntu-ji-amd64.iso`. The build copies only source/configuration files and excludes `.git`, `target`, `build`, `node_modules`, credentials and local environment files.

The chroot hook downloads the pinned Rust 1.89.0 toolchain into temporary build directories because Ubuntu 24.04's packaged Rust is too old for the runtime. Those temporary directories are removed when the hook exits; allow HTTPS access to `static.rust-lang.org` and `crates.io` during the build.

## Build from GitHub Actions

The manual **Build SolOS Ubuntu JI ISO** workflow uses a self-hosted Linux x64 runner labeled `solos-iso`. This keeps live-build's package cache and staging directory between runs, while the workflow logs and downloadable ISO/checksum are available in GitHub Actions. The runner machine must remain online during the job, have at least 25 GB free disk and 8 GB RAM, and allow the runner account to use `sudo`.

Register one repository-level runner from **Settings → Actions → Runners → New self-hosted runner**, follow GitHub's Linux x64 setup instructions, and add the unique custom label `solos-iso`. Install it as a service so closing a terminal does not stop the runner. Do not put the runner registration token in the repository or workflow. This workflow is manual-only and restricted to `main`; it does not run code from pull requests.

To build, open **Actions → Build SolOS Ubuntu JI ISO → Run workflow**. Choose `--resume` to continue using the runner's persistent build state, or `--fresh` after source/configuration changes. The completed ISO and SHA-256 checksum are uploaded as an artifact for 14 days; check the repository's Actions storage allowance first, because a large ISO can exceed the included quota. The job has a six-hour limit; if it still exceeds that after the recursive staging and cache-reset issues are fixed, the build needs to be split into stages before GitHub Actions can finish it.

## Test safely

```bash
qemu-system-x86_64 -enable-kvm -m 8192 -smp 4 \
  -cdrom out/solos-ubuntu-ji-amd64.iso
```

The live user is `solos`. **SolOS Explorer** is the primary fullscreen session. `solos-setup` automatically starts the runtime and its refresh timer and selects Explorer for folders; the previous file-manager preference is retained in the user's state directory. The native shell remains available from Applications for Wallet/Ghost and other panels. The installer is available as **Install SolOS** in the applications menu, or with `solos-installer` from a terminal. Installation is never automatic.

### Functional-session integration (candidate, 2026-11-09)

The image now packages the precompiled `solos-runtime-core`, not just the daemon. A private atomic host-state file is generated before the daemon starts and refreshed every 15 seconds after each completed refresh. The native bridge reads the daemon; loading a file fallback requires explicit `SOLOS_ALLOW_SNAPSHOT_FALLBACK=1`. Neither normal startup nor the refresh button runs `cargo`.

`solos-setup --check` checks installed executables without changing configuration. Normal session startup runs setup automatically, without asking the user to edit JSON files. It does not create wallet keys, sign transactions, connect paid providers or grant Ghost additional permissions.

This is a **source/build candidate**, not proof of a newly booted ISO. See `docs/functional-session-20261109.md` for verification and the staged work still required for Wallet and the remaining prototype panels.

## Just Intelligent update loop

Inside the image:

```bash
solos-update check
solos-update verify
sudo solos-update apply
sudo solos-update rollback
```

`check` fetches metadata and shows the candidate. `verify` builds/tests in isolation. `apply` requires an explicit command and atomically switches the active release. `rollback` returns to the previous verified release. There is no silent self-modification.

## Next release gates

1. Build the ISO in CI and publish its SHA-256 plus SBOM.
2. Boot-test in QEMU and on one spare machine.
3. Add signed release metadata before enabling network updates outside development.
4. Validate the installer on a spare disk; it is intentionally user-triggered and never runs automatically.
