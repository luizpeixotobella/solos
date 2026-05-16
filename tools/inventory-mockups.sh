#!/usr/bin/env bash
set -euo pipefail
ROOT="${1:-$(pwd)}"
OUT="${2:-docs/cases/mockup-inventory.md}"
mkdir -p "$(dirname "$OUT")"
{
  echo "# Mockup and prototype inventory"
  echo
  echo "Generated: $(date -Iseconds)"
  echo
  echo "## Candidate files"
  echo
  find "$ROOT" \
    -path '*/node_modules/*' -prune -o \
    -path '*/target/*' -prune -o \
    -path '*/build/*' -prune -o \
    -type f \( \
      -iname '*mock*' -o \
      -iname '*demo*' -o \
      -iname '*.qml' -o \
      -iname '*prototype*' -o \
      -iname '*snapshot*.json' \
    \) -print | sort | sed 's#^#- #'
  echo
  echo "## Review labels"
  echo
  echo "Use one label per file: runtime, ui, demo, mock, generated, archive-candidate, keep."
} > "$OUT"
echo "Wrote $OUT"
