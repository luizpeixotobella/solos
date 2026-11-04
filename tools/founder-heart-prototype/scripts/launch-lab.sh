#!/usr/bin/env bash
set -euo pipefail
LAB_DIR="${SOLOS_HEART_LAB_DIR:-/opt/solos/labs/heart-pass}"
cd "$LAB_DIR"
export SOLOS_DEMO_PORT=8766
if curl --noproxy '*' -sS --max-time 1 -o /dev/null http://127.0.0.1:8766/api/state 2>/dev/null; then
  echo 'A porta local 8766 já está em uso. Feche o laboratório anterior antes de abrir outro.' >&2
  exit 1
fi
node src/server.mjs &
lab_pid=$!
cleanup() { kill "$lab_pid" 2>/dev/null || true; }
trap cleanup EXIT
trap 'exit 130' INT TERM
for attempt in {1..60}; do
  kill -0 "$lab_pid" 2>/dev/null || { wait "$lab_pid"; exit 1; }
  if curl --noproxy '*' -fsS --max-time 1 http://127.0.0.1:8766/api/state >/dev/null 2>&1; then
    xdg-open http://127.0.0.1:8766/ >/dev/null 2>&1 || true
    break
  fi
  sleep 0.5
done
wait "$lab_pid"
