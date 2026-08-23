#!/usr/bin/env bash
set -euo pipefail

repo_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
daemon_bin="$repo_dir/app/runtime-core/target/debug/solos-daemon"
verifier_bin="$repo_dir/app/runtime-core/target/debug/ghost-audit-verify"
snapshot="${SOLOS_SMOKE_SNAPSHOT:-$repo_dir/app/shell-native/src/runtime_snapshot.json}"

test -x "$daemon_bin"
test -x "$verifier_bin"
test -s "$snapshot"
command -v python3 >/dev/null

smoke_dir="$(mktemp -d /tmp/solos-ghost-audit-smoke.XXXXXX)"
socket_path="$smoke_dir/daemon.sock"
audit_store="$smoke_dir/ghost-audits.json"
audit_root="$smoke_dir/bundles"
daemon_log="$smoke_dir/daemon.log"
daemon_pid=""

cleanup() {
  if [ -n "$daemon_pid" ]; then
    kill "$daemon_pid" 2>/dev/null || true
    wait "$daemon_pid" 2>/dev/null || true
  fi
  rm -f "$socket_path" "$daemon_log"
  rm -rf "$audit_root"
  rm -f "$audit_store"
  rmdir "$smoke_dir" 2>/dev/null || true
}
trap cleanup EXIT

start_daemon() {
  SOLOS_DAEMON_SOCKET="$socket_path" \
  SOLOS_RUNTIME_SNAPSHOT="$snapshot" \
  SOLOS_GHOST_AUDIT_STORE="$audit_store" \
  SOLOS_GHOST_AUDIT_DIR="$audit_root" \
  SOLOS_GHOST_AUDIT_VERIFIER="$verifier_bin" \
    "$daemon_bin" >"$daemon_log" 2>&1 &
  daemon_pid=$!
  for _ in 1 2 3 4 5 6 7 8 9 10; do
    test -S "$socket_path" && return
    sleep 0.2
  done
  sed -n '1,120p' "$daemon_log" >&2
  exit 1
}

stop_daemon() {
  kill "$daemon_pid"
  wait "$daemon_pid" 2>/dev/null || true
  daemon_pid=""
  rm -f "$socket_path"
}

start_daemon
python3 - "$socket_path" "$audit_store" <<'PY'
import json
import os
import socket
import sys

socket_path, audit_store = sys.argv[1:]

def request(method, params=None):
    client = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
    client.connect(socket_path)
    client.sendall((json.dumps({"id": method, "method": method, "params": params or {}}) + "\n").encode())
    response = b""
    while not response.endswith(b"\n"):
        response += client.recv(65536)
    client.close()
    decoded = json.loads(response)
    assert decoded["ok"], decoded
    return decoded["result"]

dangerous = request("ghost.audit.prepare", {"input": "sudo rm -rf / and publish wallet secrets"})
dangerous_id = dangerous["transition"]["auditId"]
assert dangerous["transition"]["classification"]["risk"] == "critical"
denied = request("ghost.audit.decide", {"id": dangerous_id, "approved": False})
assert denied["transition"]["status"] == "blocked"
assert not denied["transition"]["artifactPath"]

prepared = request("ghost.audit.prepare", {"input": "Abra o Workspace e preserve uma prova verificável"})
audit_id = prepared["transition"]["auditId"]
assert prepared["transition"]["status"] == "awaiting-approval"
decided = request("ghost.audit.decide", {"id": audit_id, "approved": True})
artifact_path = decided["transition"]["artifactPath"]
assert os.path.isfile(artifact_path)
verified = request("ghost.audit.verify", {"id": audit_id})
assert verified["transition"]["status"] == "verified"
receipt_path = verified["transition"]["receiptPath"]
assert os.path.isfile(receipt_path)

with open(audit_store, "r", encoding="utf-8") as handle:
    store = json.load(handle)
assert store["schema"] == "solos.ghost.audits.v1"
assert any(item["id"] == audit_id and item["status"] == "verified" for item in store["audits"])
with open(os.path.join(os.path.dirname(audit_store), "verified-id"), "w", encoding="utf-8") as handle:
    handle.write(audit_id)
PY
stop_daemon

start_daemon
python3 - "$socket_path" "$smoke_dir/verified-id" <<'PY'
import json
import socket
import sys

socket_path, id_path = sys.argv[1:]
with open(id_path, "r", encoding="utf-8") as handle:
    audit_id = handle.read()
client = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
client.connect(socket_path)
client.sendall(b'{"id":"persisted","method":"ghost.audits.get","params":{}}\n')
response = b""
while not response.endswith(b"\n"):
    response += client.recv(65536)
client.close()
decoded = json.loads(response)
assert decoded["ok"], decoded
assert any(item["id"] == audit_id and item["status"] == "verified" for item in decoded["result"]["audits"])
PY
rm -f "$smoke_dir/verified-id"

echo "SolOS Ghost audit smoke passed: real input, denial, Linux artifact, external verifier, portable receipt, and Daemon restart persistence."
