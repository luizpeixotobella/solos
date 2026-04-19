# SolOS demo ISO live-build scaffold

This directory contains a real starting scaffold for building a **SolOS v1.0 demo ISO** with Debian `live-build`.

The design is intentionally honest:

- Debian/Linux provides the boot/runtime substrate
- SolOS provides the shell and operating layer
- this scaffold packages that into a bootable demo image

## What is here

- `config/package-lists/solos.list.chroot` - packages required in the image
- `config/includes.chroot/opt/solos-appliance/` - appliance files copied into the image
- `config/includes.chroot/etc/solos-build.env` - image build environment file
- `config/hooks/live/0100-solos-appliance.chroot` - customization hook executed inside the image build
- `build-iso.sh` - helper to configure and build the ISO with `live-build`
- `prepare-build-context.sh` - copies the current SolOS repo artifacts into the live-build context

## Expected flow

1. Build SolOS assets in the repo
2. Run `prepare-build-context.sh`
3. Run `build-iso.sh`
4. Test the generated ISO in a VM

## Important rule

This scaffold packages **Linux first, SolOS second**.

It does not try to build a fake standalone SolOS runtime.
