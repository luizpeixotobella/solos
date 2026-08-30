#!/usr/bin/env bash
set -euo pipefail

SOLOS_REPO="${SOLOS_REPO:-$(cd "$(dirname "${BASH_SOURCE[0]}")/../../.." && pwd)}"
RUNTIME_DIR="$SOLOS_REPO/app/runtime-core"
TARGET_ROOT="${SOLOS_RUNTIME_TARGET:-/opt/solos-runtime}"

if [[ "${EUID}" -ne 0 ]]; then
  echo "Run as root: sudo SOLOS_REPO=/path/to/solos ./bin/install-daemon-build.sh" >&2
  exit 1
fi

cd "$RUNTIME_DIR"
cargo build --release --bin solos-daemon --bin ghost-audit-verify
install -d -m 0755 "$TARGET_ROOT/bin"
install -d -m 0755 "$TARGET_ROOT/tools"
install -m 0755 target/release/solos-daemon "$TARGET_ROOT/bin/solos-daemon"
install -m 0755 target/release/ghost-audit-verify "$TARGET_ROOT/bin/ghost-audit-verify"
install -m 0755 "$SOLOS_REPO/tools/sync-ghost-brain.mjs" "$TARGET_ROOT/tools/sync-ghost-brain.mjs"
echo "Installed SolOS Daemon, Ghost audit verifier and minimized CMS sync tool to $TARGET_ROOT"
