#!/usr/bin/env bash
set -euo pipefail

URL="${SOLOS_URL:-http://127.0.0.1:8080}"
BROWSER_BIN="${SOLOS_BROWSER_BIN:-}"

find_browser() {
  if [[ -n "$BROWSER_BIN" ]] && command -v "$BROWSER_BIN" >/dev/null 2>&1; then
    command -v "$BROWSER_BIN"
    return 0
  fi

  for candidate in chromium chromium-browser google-chrome google-chrome-stable; do
    if command -v "$candidate" >/dev/null 2>&1; then
      command -v "$candidate"
      return 0
    fi
  done

  return 1
}

BROWSER="$(find_browser || true)"
if [[ -z "$BROWSER" ]]; then
  echo "No supported browser found (chromium/chrome)." >&2
  exit 1
fi

while true; do
  "$BROWSER" \
    --kiosk \
    --incognito \
    --disable-session-crashed-bubble \
    --disable-infobars \
    --check-for-update-interval=31536000 \
    --no-first-run \
    --no-default-browser-check \
    --overscroll-history-navigation=0 \
    --disable-features=Translate,InfiniteSessionRestore \
    "$URL"
  sleep 2
done
