#!/usr/bin/env bash
set -euo pipefail

repo_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
daemon_bin="$repo_dir/app/runtime-core/target/debug/solos-daemon"
shell_bin="$repo_dir/app/shell-native/build/solos-shell-native"
snapshot="${SOLOS_SMOKE_SNAPSHOT:-$repo_dir/app/shell-native/src/runtime_snapshot.json}"

test -x "$daemon_bin"
test -x "$shell_bin"
test -s "$snapshot"

smoke_dir="$(mktemp -d /tmp/solos-native-smoke.XXXXXX)"
socket_path="$smoke_dir/daemon.sock"
daemon_log="$smoke_dir/daemon.log"
shell_log="$smoke_dir/shell.log"
daemon_pid=""
shell_pid=""

cleanup() {
  if [ -n "$shell_pid" ]; then
    kill "$shell_pid" 2>/dev/null || true
    wait "$shell_pid" 2>/dev/null || true
  fi
  if [ -n "$daemon_pid" ]; then
    kill "$daemon_pid" 2>/dev/null || true
    wait "$daemon_pid" 2>/dev/null || true
  fi
  rm -f "$socket_path" "$daemon_log" "$shell_log"
  rmdir "$smoke_dir" 2>/dev/null || true
}
trap cleanup EXIT

SOLOS_DAEMON_SOCKET="$socket_path" \
SOLOS_RUNTIME_SNAPSHOT="$snapshot" \
  "$daemon_bin" >"$daemon_log" 2>&1 &
daemon_pid=$!

for _ in 1 2 3 4 5 6 7 8 9 10; do
  test -S "$socket_path" && break
  sleep 0.2
done
test -S "$socket_path"

QT_QPA_PLATFORM=offscreen \
QT_QUICK_BACKEND=software \
SOLOS_DAEMON_SOCKET="$socket_path" \
  "$shell_bin" >"$shell_log" 2>&1 &
shell_pid=$!

sleep 4
if ! kill -0 "$shell_pid" 2>/dev/null; then
  sed -n '1,200p' "$shell_log" >&2
  echo "Native shell exited during smoke launch." >&2
  exit 1
fi

if rg -n "ReferenceError|TypeError|QQmlApplicationEngine failed|module .* is not installed" "$shell_log"; then
  echo "Native shell emitted a QML runtime error." >&2
  exit 1
fi

echo "SolOS native shell smoke passed."
