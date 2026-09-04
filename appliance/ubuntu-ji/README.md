# SolOS Ubuntu JI image

This directory builds a bootable **Ubuntu 24.04 (Noble) live ISO** with:

- the native SolOS Qt/QML shell;
- the Rust SolOS runtime and Ghost verifier;
- GNOME Terminal and a normal Ubuntu desktop beneath SolOS;
- the SolOS source tree in `/opt/solos-src` for local development;
- `solos-update`, an approval-bound update helper with verify/apply/rollback stages.

It is a test/development appliance, not yet a replacement for a general-purpose production OS. Linux remains the base; SolOS is the operating and intelligence layer above it.

## Build

Build on Ubuntu/Debian with at least 25 GB free disk and 8 GB RAM:

```bash
sudo apt-get install live-build debootstrap xorriso squashfs-tools rsync
sudo ./build-iso.sh
```

The result is written to `out/solos-ubuntu-ji-amd64.iso`. The build copies only source/configuration files and excludes `.git`, `target`, `build`, `node_modules`, credentials and local environment files.

## Test safely

```bash
qemu-system-x86_64 -enable-kvm -m 8192 -smp 4 \
  -cdrom out/solos-ubuntu-ji-amd64.iso
```

The live user is `solos`. The native shell starts after the desktop session; a terminal remains available from the applications menu.

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
4. Add an installer only after the live image proves stable; the first artifact is deliberately non-destructive.
