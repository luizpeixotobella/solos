#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"
SOL_USER="${SOL_USER:-solos}"
SOL_HOME="${SOL_HOME:-/home/$SOL_USER}"
SYSTEMD_USER_DIR="$SOL_HOME/.config/systemd/user"
AUTOSTART_DIR="$SOL_HOME/.config/autostart"
SOLOS_CONFIG_DIR="$SOL_HOME/.config/solos"
XSESSION_TARGET="$SOL_HOME/.xsession"
ENV_TARGET="/etc/solos-shell.env"

if [[ "${EUID}" -ne 0 ]]; then
  echo "Run as root: sudo ./bin/provision-demo-host.sh" >&2
  exit 1
fi

install -d -m 0755 "$SYSTEMD_USER_DIR" "$AUTOSTART_DIR" "$SOLOS_CONFIG_DIR" /opt/solos-shell/bin /opt/solos-shell/current
install -m 0644 "$ROOT_DIR/config/systemd/solos-shell.service" "$SYSTEMD_USER_DIR/solos-shell.service"
install -m 0755 "$ROOT_DIR/config/x11/solos-xsession.sh" "$XSESSION_TARGET"
install -m 0755 "$ROOT_DIR/bin/launch-kiosk.sh" "$SOLOS_CONFIG_DIR/launch-kiosk.sh"
install -m 0644 "$ROOT_DIR/config/autostart/solos-kiosk.desktop" "$AUTOSTART_DIR/solos-kiosk.desktop"
install -m 0644 "$ROOT_DIR/config/environment/solos-shell.env.example" "$ENV_TARGET"
chown -R "$SOL_USER:$SOL_USER" "$SOL_HOME/.config" "$XSESSION_TARGET"

cat <<EOF
Provisioned SolOS demo appliance files for user '$SOL_USER'.

Next steps:
1. install OS packages: xorg openbox chromium nodejs rsync
2. build the shell from the repo
3. run ./bin/install-shell-build.sh
4. log in as $SOL_USER and enable the user service:
   systemctl --user daemon-reload
   systemctl --user enable --now solos-shell.service
5. configure graphical autologin for your display manager
6. set the session to use ~/.xsession or use the autostart desktop entry
EOF
