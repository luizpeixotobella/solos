#!/usr/bin/env bash
set -euo pipefail

if command -v systemctl >/dev/null 2>&1; then
  systemctl --user import-environment DISPLAY XAUTHORITY XDG_RUNTIME_DIR DBUS_SESSION_BUS_ADDRESS || true
  systemctl --user start solos-shell.service || true
fi

if command -v openbox-session >/dev/null 2>&1; then
  openbox-session &
fi

exec "$HOME/.config/solos/launch-kiosk.sh"
