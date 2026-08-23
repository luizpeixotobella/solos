#!/usr/bin/env bash
set -euo pipefail

repo_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
runtime_dir="$repo_dir/app/runtime-core"
daemon_bin="$runtime_dir/target/debug/solos-daemon"
verifier_bin="$runtime_dir/target/debug/ghost-audit-verify"
snapshot="$repo_dir/app/shell-native/src/runtime_snapshot.json"
approve=false
output_root="$repo_dir/ghost-audit-output"
input_parts=()

while [ "$#" -gt 0 ]; do
  case "$1" in
    --approve)
      approve=true
      shift
      ;;
    --output)
      test "$#" -ge 2 || { echo "--output requires a directory" >&2; exit 64; }
      output_root="$2"
      shift 2
      ;;
    --help|-h)
      echo "usage: tools/ghost-audit-pilot.sh [--approve] [--output DIR] <input>"
      exit 0
      ;;
    *)
      input_parts+=("$1")
      shift
      ;;
  esac
done

if [ "${#input_parts[@]}" -eq 0 ]; then
  if [ ! -t 0 ]; then
    echo "Ghost audit input is required" >&2
    exit 64
  fi
  read -r -p "Input para o Ghost auditar: " audit_input
else
  audit_input="${input_parts[*]}"
fi

test -n "${audit_input//[[:space:]]/}" || { echo "Ghost audit input is required" >&2; exit 64; }
command -v cargo >/dev/null
command -v jq >/dev/null
command -v python3 >/dev/null
command -v sha256sum >/dev/null

(cd "$runtime_dir" && cargo build --quiet --bin solos-daemon --bin ghost-audit-verify)

run_stamp="$(date -u +%Y%m%dT%H%M%S%NZ)"
output_dir="$output_root/$run_stamp"
mkdir -p "$output_dir"
session_dir="$(mktemp -d /tmp/solos-ghost-audit-pilot.XXXXXX)"
socket_path="$session_dir/daemon.sock"
daemon_log="$session_dir/daemon.log"
daemon_pid=""

cleanup() {
  if [ -n "$daemon_pid" ]; then
    kill "$daemon_pid" 2>/dev/null || true
    wait "$daemon_pid" 2>/dev/null || true
  fi
  rm -f "$socket_path" "$daemon_log"
  rmdir "$session_dir" 2>/dev/null || true
}
trap cleanup EXIT

SOLOS_DAEMON_SOCKET="$socket_path" \
SOLOS_RUNTIME_SNAPSHOT="$snapshot" \
SOLOS_GHOST_AUDIT_STORE="$output_dir/store.json" \
SOLOS_GHOST_AUDIT_DIR="$output_dir/bundles" \
SOLOS_GHOST_AUDIT_VERIFIER="$verifier_bin" \
  "$daemon_bin" >"$daemon_log" 2>&1 &
daemon_pid=$!

for _ in 1 2 3 4 5 6 7 8 9 10; do
  test -S "$socket_path" && break
  sleep 0.2
done
test -S "$socket_path" || { sed -n '1,120p' "$daemon_log" >&2; exit 1; }

prepared_json="$(python3 - "$socket_path" "$audit_input" <<'PY'
import json
import socket
import sys

socket_path, audit_input = sys.argv[1:]
client = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
client.connect(socket_path)
client.sendall((json.dumps({
    "id": "pilot-prepare",
    "method": "ghost.audit.prepare",
    "params": {"input": audit_input},
}, ensure_ascii=False) + "\n").encode())
response = b""
while not response.endswith(b"\n"):
    response += client.recv(65536)
client.close()
decoded = json.loads(response)
if not decoded.get("ok"):
    raise SystemExit(decoded.get("error", "Ghost audit prepare failed"))
print(json.dumps(decoded["result"]["transition"], ensure_ascii=False))
PY
)"

audit_id="$(jq -r '.auditId' <<<"$prepared_json")"
echo
echo "Ghost classified the exact input without executing it:"
jq '{auditId,status,inputSha256,classification}' <<<"$prepared_json"
echo

if [ "$approve" != true ]; then
  read -r -p "Approve ONLY the isolated proof artifact write? [y/N] " answer
  case "$answer" in
    y|Y|yes|YES|sim|SIM) approve=true ;;
  esac
fi

if [ "$approve" != true ]; then
  python3 - "$socket_path" "$audit_id" <<'PY'
import json
import socket
import sys

socket_path, audit_id = sys.argv[1:]
client = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
client.connect(socket_path)
client.sendall((json.dumps({
    "id": "pilot-deny",
    "method": "ghost.audit.decide",
    "params": {"id": audit_id, "approved": False},
}) + "\n").encode())
response = b""
while not response.endswith(b"\n"):
    response += client.recv(65536)
client.close()
decoded = json.loads(response)
if not decoded.get("ok"):
    raise SystemExit(decoded.get("error", "Ghost audit denial failed"))
print(json.dumps(decoded["result"]["transition"], indent=2))
PY
  echo "Audit denied. No proof artifact was written. Store: $output_dir/store.json"
  exit 0
fi

python3 - "$socket_path" "$audit_id" <<'PY'
import json
import socket
import sys

socket_path, audit_id = sys.argv[1:]

def request(method, params):
    client = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
    client.connect(socket_path)
    client.sendall((json.dumps({"id": method, "method": method, "params": params}) + "\n").encode())
    response = b""
    while not response.endswith(b"\n"):
        response += client.recv(65536)
    client.close()
    decoded = json.loads(response)
    if not decoded.get("ok"):
        raise SystemExit(decoded.get("error", f"{method} failed"))
    return decoded["result"]

decided = request("ghost.audit.decide", {"id": audit_id, "approved": True})
assert decided["transition"]["status"] == "executed-awaiting-verification", decided
verified = request("ghost.audit.verify", {"id": audit_id})
assert verified["transition"]["status"] == "verified", verified
print(json.dumps(verified["transition"], indent=2, ensure_ascii=False))
PY

artifact_path="$(jq -r --arg id "$audit_id" '.audits[] | select(.id == $id) | .artifactPath' "$output_dir/store.json")"
receipt_path="$(jq -r --arg id "$audit_id" '.audits[] | select(.id == $id) | .receiptPath' "$output_dir/store.json")"

echo
echo "PASS — Ghost audit bundle created and independently verified."
echo "Artifact: $artifact_path"
echo "Receipt:  $receipt_path"
echo "Store:    $output_dir/store.json"
echo
echo "Shareable hashes (do not share sensitive artifact text):"
sha256sum "$artifact_path" "$receipt_path"
echo
echo "Anyone can re-run the independent verifier:"
echo "$verifier_bin $artifact_path"
echo
echo "Return your human verdict (never paste secrets or sensitive input):"
echo "https://github.com/luizpeixotobella/solos/issues/new?template=ghost-audit-pilot.yml"
