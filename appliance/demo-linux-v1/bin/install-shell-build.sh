#!/usr/bin/env bash
set -euo pipefail

SOLOS_REPO="${SOLOS_REPO:-$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)}"
BUILD_DIR="${SOLOS_BUILD_DIR:-$SOLOS_REPO/app/shell/dist}"
TARGET_ROOT="${SOLOS_TARGET_ROOT:-/opt/solos-shell}"
TARGET_DIR="$TARGET_ROOT/current"
SERVER_SCRIPT_SOURCE="$SOLOS_REPO/appliance/demo-linux-v1/bin/run-shell-server.mjs"
SERVER_SCRIPT_TARGET="$TARGET_ROOT/bin/run-shell-server.mjs"

if [[ ! -d "$BUILD_DIR" ]]; then
  echo "Build directory not found: $BUILD_DIR" >&2
  echo "Run: (cd $SOLOS_REPO/app/shell && npm install && npm run build)" >&2
  exit 1
fi

install -d "$TARGET_DIR" "$TARGET_ROOT/bin"
rsync -a --delete "$BUILD_DIR/" "$TARGET_DIR/"
install -m 0755 "$SERVER_SCRIPT_SOURCE" "$SERVER_SCRIPT_TARGET"

echo "Installed SolOS shell build to $TARGET_DIR"
echo "Installed server script to $SERVER_SCRIPT_TARGET"
