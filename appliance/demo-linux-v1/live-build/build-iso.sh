#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
WORK_DIR="${WORK_DIR:-$SCRIPT_DIR/work}"
IMAGE_NAME="${IMAGE_NAME:-solos-demo-v1}"
ARCH="${ARCH:-amd64}"
DISTRIBUTION="${DISTRIBUTION:-bookworm}"

if ! command -v lb >/dev/null 2>&1; then
  echo "live-build not found. Install it first: sudo apt install -y live-build" >&2
  exit 1
fi

mkdir -p "$WORK_DIR"
rsync -a --delete \
  --exclude work \
  --exclude .git \
  "$SCRIPT_DIR/" "$WORK_DIR/"

cd "$WORK_DIR"
rm -f .build/config/bootstrap .build/config/common .build/config/chroot .build/config/binary .build/config/source || true

lb config \
  --mode debian \
  --distribution "$DISTRIBUTION" \
  --architectures "$ARCH" \
  --binary-images iso-hybrid \
  --debian-installer live \
  --archive-areas "main contrib non-free non-free-firmware" \
  --iso-application "$IMAGE_NAME" \
  --iso-volume "$IMAGE_NAME"

sudo lb build

echo
echo "If successful, the ISO should be under:"
echo "  $WORK_DIR/live-image-$ARCH.hybrid.iso"
