#!/usr/bin/env bash
set -euo pipefail
INVENTORY="${1:-docs/cases/mockup-inventory.md}"
OUT="${2:-docs/cases/demo-cleanup-request.md}"
mkdir -p "$(dirname "$OUT")"
{
  echo "# Demo cleanup request"
  echo
  echo "Generated: $(date -Iseconds)"
  echo
  echo "## Goal"
  echo
  echo "Prepare SolOS demo 1.5/2.0 by separating source, mockups, generated artifacts, and archive candidates."
  echo
  echo "## Source inventory"
  echo
  echo "Based on: $INVENTORY"
  echo
  echo "## Required human/engineer decisions"
  echo
  echo "- Which mockups still answer a current product question?"
  echo "- Which generated artifacts should stop appearing in review?"
  echo "- Which demo surfaces must map to runtime contracts before demo 2.0?"
  echo "- Which files need backup before archive/removal?"
  echo
  echo "## Recommended cleanup policy"
  echo
  echo "1. Backup before moving/removing."
  echo "2. Archive stale mockups instead of deleting blindly."
  echo "3. Keep source and generated build artifacts visibly separate."
  echo "4. Require a build/smoke gate after each cleanup slice."
} > "$OUT"
echo "Wrote $OUT"
