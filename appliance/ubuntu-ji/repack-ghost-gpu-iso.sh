#!/usr/bin/env bash
# Additive lab derivative of the already-verified 8506a00 ISO. Never mutates it.
set -euo pipefail
umask 022
base=${SOLOS_BASE_ISO:?Set the verified base ISO path}
work=${SOLOS_GPU_WORK:?Set a NEW work directory}
out=${SOLOS_GPU_OUT:?Set a NEW output directory}
payload=${1:?source archive required}
payload_sha=${2:?source archive SHA256 required}
[[ $EUID = 0 && ! -e $work && ! -e $out ]]
install -d /run/lock
exec 9>/run/lock/solos-iso-build.lock
flock -n 9
printf '%s  %s\n' a3f9a1e61bd2e6b3e609b1c6bd0c346e9de8161634490b42f754352f8089a09a "$base" | sha256sum -c -
printf '%s  %s\n' "$payload_sha" "$payload" | sha256sum -c -
install -d "$work" "$out"
xorriso -indev "$base" -report_el_torito plain >"$out/base-boot.txt" 2>&1
xorriso -osirrox on -indev "$base" -extract /casper/filesystem.squashfs "$work/base.squashfs"
unsquashfs -processors 2 -d "$work/rootfs" "$work/base.squashfs"
root=$work/rootfs
stage=$work/source
install -d "$stage"
tar -xf "$payload" -C "$stage"
test -f "$stage/appliance/ubuntu-ji/solos-ghost-setup"
for package in libvulkan1 mesa-vulkan-drivers libgomp1 libcurl4t64; do
 test "$(dpkg-query --admindir="$root/var/lib/dpkg" -W -f='${Status}' "$package")" = 'install ok installed'
done
# No compiled binary changes, package installation, kernel/driver or boot changes.
rsync -a --exclude='__pycache__' "$stage/" "$root/opt/solos-src/"
for script in solos-ghost-setup solos-ghost-setup-ui; do
 install -m 0755 "$stage/appliance/ubuntu-ji/$script" "$root/usr/local/bin/$script"
done
for unit in solos-ghost-local.service solos-ghost-api.service; do
 install -m 0644 "$stage/appliance/ubuntu-ji/$unit" "$root/etc/systemd/user/$unit"
done
install -m 0644 "$stage/appliance/ubuntu-ji/solos-ghost-setup.desktop" "$root/usr/share/applications/"
chroot "$root" /usr/bin/python3 -m unittest discover -s /opt/solos-src/appliance/ubuntu-ji/tests -v
chroot "$root" /usr/local/bin/solos-ghost-setup --check
chroot "$root" /usr/bin/env SYSTEMD_LOG_LEVEL=err systemd-analyze verify /etc/systemd/user/solos-ghost-local.service /etc/systemd/user/solos-ghost-api.service
# Don't bake executed Python cache or owner profiles into the image.
find "$root/opt/solos-src/appliance/ubuntu-ji" -type d -name __pycache__ -exec rm -r {} +
if find "$root/root" "$root/home" -type f \( -name ghost-api.json -o -name engine.key -o -name wallet.json \) -print -quit | grep -q .; then
 echo 'Unexpected private user configuration in image' >&2; exit 1
fi
du -sx --block-size=1 "$root" | cut -f1 >"$work/filesystem.size"
# gzip is deliberate for a lab repack: fast packaging, larger than the xz base.
mksquashfs "$root" "$work/filesystem.squashfs" -noappend -comp gzip -processors 2 -no-progress
smoke=$(mktemp -d "$root/tmp/solos-gpu-smoke.XXXXXX")
inner=${smoke#"$root"}
install -d -m 0700 "$smoke/runtime" "$smoke/config"
for gui in solos-shell-native solos-explorer-prototype; do
 unsquashfs -cat "$work/filesystem.squashfs" "opt/solos/releases/bootstrap/bin/$gui" >"$smoke/$gui"
 unsquashfs -cat "$work/base.squashfs" "opt/solos/releases/bootstrap/bin/$gui" >"$smoke/$gui.base"
 cmp "$smoke/$gui" "$smoke/$gui.base"
 chmod 755 "$smoke/$gui"
 rc=0
 chroot "$root" /usr/bin/env -i PATH=/usr/bin:/bin HOME=/root QT_QPA_PLATFORM=offscreen QT_QUICK_BACKEND=software XDG_RUNTIME_DIR="$inner/runtime" XDG_CONFIG_HOME="$inner/config" timeout 6 "$inner/$gui" >"$out/$gui.log" 2>&1 || rc=$?
 test "$rc" = 124
 ! grep -Ei 'failed to load component|is not a type|module .* is not installed|TypeError:|ReferenceError:|Cannot assign' "$out/$gui.log"
 echo "FINAL_GUI_PASS $gui"
done
unsquashfs -cat "$work/filesystem.squashfs" usr/local/bin/solos-ghost-setup >"$out/packaged-setup.py"
cmp "$out/packaged-setup.py" "$stage/appliance/ubuntu-ji/solos-ghost-setup"
xorriso -osirrox on -indev "$base" -extract /SHA256SUMS "$work/base-SHA256SUMS"
python3 - "$work" <<'PY'
import hashlib,pathlib,sys
w=pathlib.Path(sys.argv[1]); text=(w/'base-SHA256SUMS').read_text()
for name in ('filesystem.squashfs','filesystem.size'):
 with (w/name).open('rb') as f: digest=hashlib.file_digest(f,'sha256').hexdigest()
 lines=text.splitlines(); count=0
 for i,line in enumerate(lines):
  if line.endswith('  ./casper/'+name):
   lines[i]=digest+'  ./casper/'+name; count+=1
 if count != 1: raise RuntimeError('Missing or ambiguous embedded manifest entry')
 text='\n'.join(lines)+'\n'
(w/'SHA256SUMS').write_text(text)
PY
iso=$out/solos-ghost-gpu-amd64.iso
xorriso -indev "$base" -outdev "$iso" -boot_image any replay \
 -map "$work/filesystem.squashfs" /casper/filesystem.squashfs \
 -map "$work/filesystem.size" /casper/filesystem.size \
 -map "$work/SHA256SUMS" /SHA256SUMS
xorriso -indev "$iso" -report_el_torito plain >"$out/boot.txt" 2>&1
grep -q 'BIOS' "$out/boot.txt"
grep -q 'UEFI' "$out/boot.txt"
xorriso -osirrox on -indev "$iso" -extract / "$work/iso-integrity-check"
(cd "$work/iso-integrity-check"; sha256sum -c SHA256SUMS) >"$out/embedded-integrity.log" 2>&1
file "$iso"
fdisk -l "$iso"
(cd "$out"; sha256sum solos-ghost-gpu-amd64.iso >SHA256SUMS; sha256sum -c SHA256SUMS)
echo 'PACKAGED_OK: guest boot and physical Linux GPU acceptance remain separate.'
