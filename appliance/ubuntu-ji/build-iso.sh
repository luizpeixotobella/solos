#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
SOLOS_REPO="${SOLOS_REPO:-$(cd "$SCRIPT_DIR/../.." && pwd)}"
BUILD_DIR="${SOLOS_ISO_BUILD_DIR:-$SCRIPT_DIR/.build}"
OUT_DIR="${SOLOS_ISO_OUT_DIR:-$SCRIPT_DIR/out}"

if [[ "${EUID}" -ne 0 ]]; then
  echo "Run with sudo: sudo $0" >&2
  exit 1
fi
for command in lb debootstrap xorriso mksquashfs rsync; do
  command -v "$command" >/dev/null || { echo "Missing command: $command" >&2; exit 1; }
done
[[ -f "$SOLOS_REPO/app/runtime-core/Cargo.toml" ]] || { echo "Invalid SOLOS_REPO: $SOLOS_REPO" >&2; exit 1; }

install -d -m 0755 "$BUILD_DIR" "$OUT_DIR"
cd "$BUILD_DIR"
lb clean --purge || true
lb config \
  --mode ubuntu \
  --distribution noble \
  --architectures amd64 \
  --archive-areas "main restricted universe multiverse" \
  --binary-images iso-hybrid \
  --bootappend-live "boot=casper components username=solos hostname=solos locales=pt_BR.UTF-8 keyboard-layouts=br" \
  --memtest none

install -d config/package-lists config/includes.chroot/opt/solos-src config/includes.chroot/usr/local/bin
cp "$SCRIPT_DIR/packages.list.chroot" config/package-lists/solos.list.chroot
rsync -a --delete \
  --exclude='.git' --exclude='.env*' --exclude='target' --exclude='build' \
  --exclude='node_modules' --exclude='*.token' --exclude='*.secret' \
  "$SOLOS_REPO/" config/includes.chroot/opt/solos-src/
install -m 0755 "$SCRIPT_DIR/solos-update" config/includes.chroot/usr/local/bin/solos-update
install -d config/hooks/normal
install -m 0755 "$SCRIPT_DIR/010-build-solos.hook.chroot" config/hooks/normal/010-build-solos.hook.chroot

lb build
ISO_PATH="$(find . -maxdepth 1 -type f -name '*.hybrid.iso' -o -name '*.iso' | head -n 1)"
[[ -n "$ISO_PATH" ]] || { echo "ISO output not found" >&2; exit 1; }
install -m 0644 "$ISO_PATH" "$OUT_DIR/solos-ubuntu-ji-amd64.iso"
sha256sum "$OUT_DIR/solos-ubuntu-ji-amd64.iso" > "$OUT_DIR/solos-ubuntu-ji-amd64.iso.sha256"
echo "Built: $OUT_DIR/solos-ubuntu-ji-amd64.iso"
