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
for command in lb debootstrap genisoimage xorriso grep-aptavail grub-mkimage grub-mkstandalone mksquashfs rsync fdisk mkfs.msdos mmd mcopy; do
  command -v "$command" >/dev/null || { echo "Missing command: $command" >&2; exit 1; }
done
[[ -f "$SOLOS_REPO/app/runtime-core/Cargo.toml" ]] || { echo "Invalid SOLOS_REPO: $SOLOS_REPO" >&2; exit 1; }
GRUB2_MBR="/usr/lib/grub/i386-pc/boot_hybrid.img"
[[ -r "$GRUB2_MBR" ]] || { echo "Missing GRUB2 hybrid MBR: $GRUB2_MBR" >&2; exit 1; }
GRUB2_CDBOOT="/usr/lib/grub/i386-pc/cdboot.img"
[[ -r "$GRUB2_CDBOOT" ]] || { echo "Missing GRUB2 El Torito boot image: $GRUB2_CDBOOT" >&2; exit 1; }

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
  --bootloader grub2 \
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

# The stock BIOS core has a path-only prefix. That works on optical media but
# can leave GRUB without a root device after the ISO is written to USB. Embed a
# small discovery config in a replacement El Torito image so both paths locate
# the ISO filesystem before loading the normal configuration and its modules.
BIOS_WORK_DIR="$BUILD_DIR/bios"
BIOS_CONFIG="$BIOS_WORK_DIR/grub.cfg"
BIOS_CORE="$BIOS_WORK_DIR/core.img"
BIOS_ELTORITO="$BUILD_DIR/binary/boot/grub/grub_eltorito"
install -d -m 0755 "$BIOS_WORK_DIR" "$BUILD_DIR/binary/boot/grub"
cat > "$BIOS_CONFIG" <<'EOF'
search --file --set=root /.disk/info
set prefix=($root)/boot/grub
configfile ($root)/boot/grub/grub.cfg
EOF
grub-mkimage \
  -d /usr/lib/grub/i386-pc \
  -p /boot/grub \
  -c "$BIOS_CONFIG" \
  -o "$BIOS_CORE" \
  biosdisk iso9660 search search_fs_file configfile normal linux tga font gfxterm all_video
cat "$GRUB2_CDBOOT" "$BIOS_CORE" > "$BIOS_ELTORITO"

# Noble's live-build 3.x can create the BIOS GRUB2 tree, but it predates the
# EFI image helper. Build a standalone x86_64 EFI loader and a small FAT image
# ourselves, then add both boot entries with xorriso below.
EFI_WORK_DIR="$BUILD_DIR/efi"
EFI_CONFIG="$EFI_WORK_DIR/grub.cfg"
EFI_BINARY="$EFI_WORK_DIR/BOOTX64.EFI"
EFI_IMAGE="$BUILD_DIR/binary/boot/grub/efi.img"
install -d -m 0755 "$EFI_WORK_DIR" "$BUILD_DIR/binary/EFI/BOOT" "$BUILD_DIR/binary/boot/grub"
cat > "$EFI_CONFIG" <<'EOF'
search --file --set=root /.disk/info
set prefix=($root)/boot/grub
configfile ($root)/boot/grub/grub.cfg
EOF
grub-mkstandalone \
  -d /usr/lib/grub/x86_64-efi \
  -O x86_64-efi \
  -o "$EFI_BINARY" \
  --modules='part_gpt part_msdos fat iso9660 search search_fs_file configfile normal linux chain' \
  "boot/grub/grub.cfg=$EFI_CONFIG"
dd if=/dev/zero of="$EFI_IMAGE" bs=1M count=16 status=none
mkfs.msdos "$EFI_IMAGE" >/dev/null
mmd -i "$EFI_IMAGE" ::EFI ::EFI/BOOT
mcopy -i "$EFI_IMAGE" "$EFI_BINARY" ::EFI/BOOT/BOOTX64.EFI
install -m 0644 "$EFI_BINARY" "$BUILD_DIR/binary/EFI/BOOT/BOOTX64.EFI"

# Rebuild the final image from the assembled binary tree. The first xorriso
# pass creates BIOS+UEFI El Torito entries; the second adds the GRUB2 hybrid
# MBR and partition table required when the ISO is written directly to USB.
OUTPUT_ISO="$OUT_DIR/solos-ubuntu-ji-amd64.iso"
BASE_ISO="$BUILD_DIR/solos-ubuntu-ji-base.iso"
rm -f "$BASE_ISO" "$OUTPUT_ISO" "$OUTPUT_ISO.sha256"
xorriso -as mkisofs \
  -R -r -J -joliet-long -l -iso-level 3 \
  -V "SOLOS_UBUNTU_JI" \
  -b boot/grub/grub_eltorito \
  -c boot.catalog \
  -no-emul-boot \
  -boot-load-size 4 \
  -boot-info-table \
  -eltorito-alt-boot \
  -e boot/grub/efi.img \
  -no-emul-boot \
  -isohybrid-gpt-basdat \
  -isohybrid-apm-hfsplus \
  -o "$BASE_ISO" \
  "$BUILD_DIR/binary"
xorriso -indev "$BASE_ISO" \
  -outdev "$OUTPUT_ISO" \
  -boot_image any discard \
  -boot_image any bin_path=/boot/grub/grub_eltorito \
  -boot_image any boot_info_table=on \
  -boot_image any grub2_boot_info=on \
  -boot_image any cat_path=boot.catalog \
  -boot_image any next \
  -boot_image any efi_path=/boot/grub/efi.img \
  -boot_image any grub2_mbr="$GRUB2_MBR" \
  -boot_image any partition_table=on \
  -boot_image any partition_cyl_align=all \
  -commit

[[ -s "$OUTPUT_ISO" ]] || { echo "xorriso did not create the output ISO" >&2; exit 1; }
if ! xorriso -indev "$OUTPUT_ISO" -ls /EFI/BOOT/BOOTX64.EFI 2>/dev/null | grep -qi 'BOOTX64\.EFI'; then
  echo "UEFI BOOTX64.EFI missing from output ISO" >&2
  exit 1
fi
ELTORITO_REPORT="$BUILD_DIR/el-torito.txt"
xorriso -indev "$OUTPUT_ISO" -report_el_torito plain > "$ELTORITO_REPORT"
grep -q 'BIOS' "$ELTORITO_REPORT" || { echo "BIOS El Torito entry missing from output ISO" >&2; exit 1; }
grep -q 'UEFI' "$ELTORITO_REPORT" || { echo "UEFI El Torito entry missing from output ISO" >&2; exit 1; }
fdisk -l "$OUTPUT_ISO"
if ! fdisk -l "$OUTPUT_ISO" | grep -Eq 'Disklabel type: (dos|gpt)|^Device'; then
  echo "No USB partition table detected in output ISO" >&2
  exit 1
fi
sha256sum "$OUTPUT_ISO" > "$OUTPUT_ISO.sha256"
echo "Built: $OUTPUT_ISO"
