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
for command in lb debootstrap genisoimage xorriso grep-aptavail grub-mkimage mksquashfs rsync fdisk; do
  command -v "$command" >/dev/null || { echo "Missing command: $command" >&2; exit 1; }
done
[[ -f "$SOLOS_REPO/app/runtime-core/Cargo.toml" ]] || { echo "Invalid SOLOS_REPO: $SOLOS_REPO" >&2; exit 1; }
GRUB2_MBR="/usr/lib/grub/i386-pc/boot_hybrid.img"
[[ -r "$GRUB2_MBR" ]] || { echo "Missing GRUB2 hybrid MBR: $GRUB2_MBR" >&2; exit 1; }

# Ubuntu Noble ships live-build 3.x, whose GRUB2 ISO helper predates the
# mandatory prefix argument in GRUB 2.12. Patch that single generated command
# on the ephemeral build host; fail closed if the expected helper is absent.
LIVE_BUILD_BINARY_ISO="/usr/lib/live/build/lb_binary_iso"
[[ -f "$LIVE_BUILD_BINARY_ISO" ]] || { echo "Missing live-build GRUB2 ISO helper" >&2; exit 1; }
if grep -Fq 'grub-mkimage -d \${input_dir} -o \${core_img}' "$LIVE_BUILD_BINARY_ISO"; then
  sed -i 's|grub-mkimage -d \\${input_dir} -o \\${core_img}|grub-mkimage -d \\${input_dir} -p /boot/grub -o \\${core_img}|' "$LIVE_BUILD_BINARY_ISO"
fi
grep -Fq 'grub-mkimage -d \${input_dir} -p /boot/grub -o \${core_img}' "$LIVE_BUILD_BINARY_ISO" || {
  echo "Unsupported live-build GRUB2 helper format" >&2
  exit 1
}

install -d -m 0755 "$BUILD_DIR" "$OUT_DIR"
cd "$BUILD_DIR"
lb clean --purge || true
lb config \
  --mode ubuntu \
  --distribution noble \
  --architectures amd64 \
  --archive-areas "main restricted universe multiverse" \
  --binary-images iso \
  --build-with-chroot false \
  --bootloaders grub-pc,grub-efi \
  --bootappend-live "boot=casper components username=solos hostname=solos locales=pt_BR.UTF-8 keyboard-layouts=br" \
  --debian-installer false \
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
# xorriso creates the ISO and preserves its BIOS+UEFI El Torito entries, but
# the GRUB2 MBR and partition table must be added explicitly for USB boot.
OUTPUT_ISO="$OUT_DIR/solos-ubuntu-ji-amd64.iso"
rm -f "$OUTPUT_ISO" "$OUTPUT_ISO.sha256"
xorriso -indev "$ISO_PATH" \
  -outdev "$OUTPUT_ISO" \
  -boot_image any replay \
  -boot_image any grub2_mbr="$GRUB2_MBR" \
  -boot_image any partition_table=on \
  -boot_image any partition_cyl_align=all \
  -commit

[[ -s "$OUTPUT_ISO" ]] || { echo "xorriso did not create the output ISO" >&2; exit 1; }
EFI_BOOT="$(xorriso -indev "$OUTPUT_ISO" -find / -type f -print 2>/dev/null | grep -i '/EFI/BOOT/BOOTX64\.EFI' || true)"
[[ -n "$EFI_BOOT" ]] || { echo "UEFI BOOTX64.EFI missing from output ISO" >&2; exit 1; }
fdisk -l "$OUTPUT_ISO"
if ! fdisk -l "$OUTPUT_ISO" | grep -Eq 'Disklabel type: (dos|gpt)|^Device'; then
  echo "No USB partition table detected in output ISO" >&2
  exit 1
fi
sha256sum "$OUTPUT_ISO" > "$OUTPUT_ISO.sha256"
echo "Built: $OUTPUT_ISO"
