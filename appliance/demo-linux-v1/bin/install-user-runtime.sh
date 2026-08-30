#!/usr/bin/env bash
set -euo pipefail

SOLOS_REPO="${SOLOS_REPO:-$(cd "$(dirname "${BASH_SOURCE[0]}")/../../.." && pwd)}"
runtime_dir="$SOLOS_REPO/app/runtime-core"
target_root="${SOLOS_USER_RUNTIME_TARGET:-${HOME:?}/.local/lib/solos}"
unit_root="${HOME:?}/.config/systemd/user"

case "$target_root" in
  "${HOME:?}/.local/lib/solos"|"${HOME:?}/.local/lib/solos/"*) ;;
  *)
    echo "Refusing user install outside the owner-local .local/lib/solos tree: $target_root" >&2
    exit 1
    ;;
esac

cd "$runtime_dir"
cargo build --release --bin solos-daemon --bin ghost-audit-verify

install -d -m 0755 "$target_root/bin" "$target_root/tools" "$target_root/share" "$unit_root"
install -m 0755 target/release/solos-daemon "$target_root/bin/solos-daemon"
install -m 0755 target/release/ghost-audit-verify "$target_root/bin/ghost-audit-verify"
install -m 0755 "$SOLOS_REPO/tools/sync-ghost-brain.mjs" "$target_root/tools/sync-ghost-brain.mjs"
install -m 0644 "$SOLOS_REPO/app/shell-native/src/runtime_snapshot.json" "$target_root/share/runtime_snapshot.json"
install -m 0644 "$SOLOS_REPO/appliance/demo-linux-v1/config/systemd-user/solos-daemon.service" "$unit_root/solos-daemon.service"
install -m 0644 "$SOLOS_REPO/appliance/demo-linux-v1/config/systemd-user/solos-ghost-sync.service" "$unit_root/solos-ghost-sync.service"
install -m 0644 "$SOLOS_REPO/appliance/demo-linux-v1/config/systemd-user/solos-ghost-sync.timer" "$unit_root/solos-ghost-sync.timer"

systemctl --user daemon-reload
systemctl --user enable --now solos-daemon.service solos-ghost-sync.timer

echo "Installed and enabled the owner-local SolOS Daemon and Ghost evidence timer."
