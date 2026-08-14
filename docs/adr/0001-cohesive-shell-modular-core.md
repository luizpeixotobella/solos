# ADR 0001 — Cohesive shell, modular core

Date: 2026-08-14

Status: accepted

## Context

SolOS intentionally presents Ghost, Wallet, Apps, approvals and host state as one operating environment instead of a collection of unrelated applications. During the RC1 prototype, that product cohesion also concentrated implementation in a large Rust snapshot generator, a broad Qt `AppController` and one aggregated JSON contract.

The thesis needs an explicit distinction between user-facing cohesion and internal coupling.

## Decision

SolOS adopts a **modular-monolith** posture:

- one owner-scoped Daemon remains the persistent runtime process;
- Linux remains the base system;
- the shell remains one coherent operating experience;
- runtime internals are divided by natural ownership: host, Ghost, Wallet, Apps, approvals, quota, events and contracts;
- the shell may consume an aggregated Home snapshot, while domain contracts and events stay independently versioned;
- persistent/systemic mediation belongs in the Daemon; domain presentation and interaction remain in their native SolOS surfaces;
- microservices are not introduced without an operational reason.

The product maxim is:

> **Cohesive outside, modular inside.**

## First implementation slice

- Extract Linux host discovery into `runtime-core/src/host_runtime.rs`.
- Extract operating-surface catalog construction into `runtime-core/src/surface_catalog.rs`.
- Add module-level tests while preserving `solos.runtime.snapshot.v1` compatibility.
- Fix QML delegate bindings in Home and Agent.
- Add executable web/native smoke gates that fail on missing assets, early shell exit or QML runtime errors.
- Correct the repository-default web-shell asset path.
- Stop the shell constructor and two-second refresh timer from spawning `cargo run` and rewriting the shared snapshot; normal refresh now reads the Daemon/fallback contract, while explicit configuration mutations may request regeneration during the transition.

## Consequences

- RC1 consumers keep the same external snapshot contract.
- Internals can evolve domain by domain without a theatrical rewrite.
- The current `AppController` and remaining runtime domains are acknowledged as the next decomposition work.
- Build success alone is no longer enough; the shell must survive a real smoke launch without QML runtime errors.
