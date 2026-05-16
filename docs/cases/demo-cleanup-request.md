# Demo cleanup request

Generated: 2026-05-15T20:52:34-03:00

## Goal

Prepare SolOS demo 1.5/2.0 by separating source, mockups, generated artifacts, and archive candidates.

## Source inventory

Based on: docs/cases/mockup-inventory.md

## Required human/engineer decisions

- Which mockups still answer a current product question?
- Which generated artifacts should stop appearing in review?
- Which demo surfaces must map to runtime contracts before demo 2.0?
- Which files need backup before archive/removal?

## Recommended cleanup policy

1. Backup before moving/removing.
2. Archive stale mockups instead of deleting blindly.
3. Keep source and generated build artifacts visibly separate.
4. Require a build/smoke gate after each cleanup slice.
