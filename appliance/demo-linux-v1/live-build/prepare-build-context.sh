#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
LIVE_BUILD_DIR="$SCRIPT_DIR"
REPO_ROOT="${SOLOS_REPO:-$(cd "$SCRIPT_DIR/../../.." && pwd)}"
ARTIFACTS_DIR="$LIVE_BUILD_DIR/config/includes.chroot/opt/solos-build"
WEB_DIST_DIR="$REPO_ROOT/app/shell/dist"
NATIVE_BUILD_DIR="${SOLOS_NATIVE_BUILD_DIR:-$REPO_ROOT/app/shell-native/build}"
RUNTIME_CORE_DIR="$REPO_ROOT/app/runtime-core"
APPLIANCE_DIR="$REPO_ROOT/appliance/demo-linux-v1"

mkdir -p "$ARTIFACTS_DIR"
rm -rf "$ARTIFACTS_DIR"/*

if [[ -d "$WEB_DIST_DIR" ]]; then
  mkdir -p "$ARTIFACTS_DIR/app/shell/dist"
  rsync -a "$WEB_DIST_DIR/" "$ARTIFACTS_DIR/app/shell/dist/"
else
  echo "warning: web shell dist not found at $WEB_DIST_DIR" >&2
fi

if [[ -x "$NATIVE_BUILD_DIR/solos-shell-native" ]]; then
  mkdir -p "$ARTIFACTS_DIR/app/shell-native/build"
  install -m 0755 "$NATIVE_BUILD_DIR/solos-shell-native" "$ARTIFACTS_DIR/app/shell-native/build/solos-shell-native"
else
  echo "warning: native shell binary not found at $NATIVE_BUILD_DIR/solos-shell-native" >&2
fi

mkdir -p "$ARTIFACTS_DIR/app/runtime-core"
rsync -a \
  --exclude target \
  "$RUNTIME_CORE_DIR/" "$ARTIFACTS_DIR/app/runtime-core/"

mkdir -p "$ARTIFACTS_DIR/appliance/demo-linux-v1"
rsync -a \
  --exclude live-build \
  "$APPLIANCE_DIR/" "$ARTIFACTS_DIR/appliance/demo-linux-v1/"

echo "Prepared live-build context under: $ARTIFACTS_DIR"
