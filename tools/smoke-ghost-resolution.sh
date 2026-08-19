#!/usr/bin/env bash
set -euo pipefail

repo_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
daemon_bin="$repo_dir/app/runtime-core/target/debug/solos-daemon"
snapshot="${SOLOS_SMOKE_SNAPSHOT:-$repo_dir/app/shell-native/src/runtime_snapshot.json}"

test -x "$daemon_bin"
test -s "$snapshot"
command -v python3 >/dev/null

smoke_dir="$(mktemp -d /tmp/solos-resolution-smoke.XXXXXX)"
socket_path="$smoke_dir/daemon.sock"
state_path="$smoke_dir/ghost-resolutions.json"
daemon_log="$smoke_dir/daemon.log"
daemon_pid=""

cleanup() {
  if [ -n "$daemon_pid" ]; then
    kill "$daemon_pid" 2>/dev/null || true
    wait "$daemon_pid" 2>/dev/null || true
  fi
  rm -f "$socket_path" "$state_path" "$daemon_log"
  rmdir "$smoke_dir" 2>/dev/null || true
}
trap cleanup EXIT

start_daemon() {
  SOLOS_DAEMON_SOCKET="$socket_path" \
  SOLOS_RUNTIME_SNAPSHOT="$snapshot" \
  SOLOS_GHOST_RESOLUTION_STORE="$state_path" \
    "$daemon_bin" >"$daemon_log" 2>&1 &
  daemon_pid=$!
  for _ in 1 2 3 4 5 6 7 8 9 10; do
    test -S "$socket_path" && return
    sleep 0.2
  done
  echo "SolOS Daemon did not create its socket." >&2
  exit 1
}

stop_daemon() {
  kill "$daemon_pid"
  wait "$daemon_pid" 2>/dev/null || true
  daemon_pid=""
  rm -f "$socket_path"
}

rpc_sequence() {
  python3 - "$socket_path" "$1" <<'PY'
import json
import socket
import sys

socket_path, phase = sys.argv[1:]

def request(method, params=None):
    client = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
    client.connect(socket_path)
    payload = {"id": method, "method": method, "params": params or {}}
    client.sendall((json.dumps(payload) + "\n").encode())
    response = b""
    while not response.endswith(b"\n"):
        chunk = client.recv(65536)
        if not chunk:
            break
        response += chunk
    client.close()
    decoded = json.loads(response)
    assert decoded["ok"], decoded
    return decoded["result"]

resolution_id = "resolution-safe-workspace"
if phase == "resolve":
    initial = request("ghost.resolutions.get")
    assert initial["selectedId"] == resolution_id
    assert initial["resolutions"][0]["status"] == "selected"
    started = request("ghost.resolution.start", {"id": resolution_id})
    assert started["transition"]["status"] == "awaiting-approval"
    decided = request("ghost.resolution.decide", {"id": resolution_id, "approved": True})
    assert decided["transition"]["status"] == "resolved"
    assert decided["resolutionLoop"]["resolutions"][0]["progress"] == 100
    snapshot = request("snapshot.get")
    assert snapshot["ghost"]["resolutionLoop"]["resolutions"][0]["status"] == "resolved"
else:
    persisted = request("ghost.resolutions.get")
    resolution = persisted["resolutions"][0]
    assert resolution["status"] == "resolved"
    assert any(item["kind"] == "verification" for item in resolution["evidence"])
PY
}

start_daemon
rpc_sequence resolve
stop_daemon
start_daemon
rpc_sequence persisted

echo "SolOS Ghost resolution smoke passed, including Daemon restart persistence."
