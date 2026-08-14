#!/usr/bin/env bash
set -euo pipefail

repo_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
server_script="$repo_dir/appliance/demo-linux-v1/bin/run-shell-server.mjs"
index_file="$repo_dir/app/shell/dist/index.html"
smoke_log="$(mktemp /tmp/solos-web-smoke.XXXXXX)"
server_pid=""

cleanup() {
  if [ -n "$server_pid" ]; then
    kill "$server_pid" 2>/dev/null || true
    wait "$server_pid" 2>/dev/null || true
  fi
  rm -f "$smoke_log"
}
trap cleanup EXIT

test -s "$index_file"
smoke_port="${SOLOS_SMOKE_PORT:-18080}"
SOLOS_PORT="$smoke_port" node "$server_script" >"$smoke_log" 2>&1 &
server_pid=$!

for _ in 1 2 3 4 5 6 7 8 9 10; do
  if curl -fsS "http://127.0.0.1:$smoke_port/" | rg -q '<div id="root"></div>'; then
    echo "SolOS web shell smoke passed."
    exit 0
  fi
  sleep 0.2
done

sed -n '1,120p' "$smoke_log" >&2
echo "Web shell did not serve the built index from its repository default path." >&2
exit 1
