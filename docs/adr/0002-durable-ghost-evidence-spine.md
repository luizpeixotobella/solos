# ADR 0002: Durable Ghost evidence spine

Date: 2026-08-29

Status: accepted

## Context

Ghost activity existed across snapshot state, resolution/audit stores and CMS workflow records. The Daemon had only an in-memory event ring, so restart continuity and one ecosystem-wide progress view were missing. Copying arbitrary local payloads to the CMS would also violate the local-first boundary.

## Decision

The owner-local SolOS Daemon is the system of record for operational events. It persists a versioned, atomic and bounded sequence ledger. Domain mutations publish small lifecycle facts into that ledger.

The Daemon exposes cursor reads and a separate minimized export contract. The export contains identity, component, lifecycle stage, status, severity, timestamp and sequence, but never the local event payload. A client signs the exact CMS request body with HMAC-SHA256 and advances its local cursor only after acceptance. The CMS verifies timestamp freshness, allowlists every field, rejects personal-data flags and deduplicates by event key.

The CMS Ghost Brain Monitor is the review/observability surface. It is not the authority for local actions and cannot send arbitrary execution instructions back to the Daemon.

## Consequences

- SolOS, Ghost, Pulso, Wallet and Apps can contribute evidence without evolving as isolated islands.
- A Daemon restart no longer erases the progress trail.
- CMS compromise does not reveal local event payloads or grant system execution.
- Human feedback and model-quality evaluation remain a separate future dataset; operational logs are evidence, not automatic learning.
- The shared secret must live only in protected local configuration and the CMS deployment secret manager.
