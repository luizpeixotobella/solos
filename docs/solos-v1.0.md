# SolOS v1.0

## Definition

SolOS v1.0 is the first explicit version of SolOS as a **Linux-based operating layer above a dedicated runtime intermediary**.

It is not a new kernel and not a replacement Linux distribution.

It is:
- a shell experience
- an orchestration layer
- an agent-aware environment
- an approval and ownership surface
- a packageable Linux appliance experience

## Version rule

For v1.0, the architecture is:

- Linux as the base system
- a runtime intermediary above Linux
- SolOS as the operating layer above that runtime

SolOS does not talk to the raw host everywhere directly.
The runtime intermediary exists to turn host state and capabilities into stable SolOS-facing contracts.

## Components in v1.0

### 1. Linux base system
The underlying kernel, services, session model, networking, and filesystem come from Linux.

### 2. SolOS runtime intermediary
`app/runtime-core` reads Linux host state, mediates host capabilities, and emits structured SolOS-visible runtime data.

### 3. SolOS shell
The shell presents Home, Agent, Wallet, and Apps as one coherent operating layer.

### 4. Appliance packaging
`appliance/demo-linux-v1` contains the first credible path to a demo image/ISO.

## What changed in this correction

Before this correction, the architecture had been improved once by saying Linux was the runtime substrate.

That was closer, but still imprecise.

The more accurate v1.0 direction is:
- Linux as base system
- runtime as intermediary mediation layer
- SolOS as operating layer
- image packaging around those three layers instead of flattening runtime into Linux or inventing a fake standalone substrate

## Why the intermediary matters

Because SolOS needs a stable middle layer that can:
- read host state
- normalize it
- mediate actions and capabilities
- keep the shell from depending directly on every raw Linux detail
- give agents and approvals a controlled operational substrate

## v1.0 goals

- be architecturally honest
- boot credibly on Linux
- expose a coherent shell
- define the runtime as a real intermediary layer
- document the build and image story clearly
- create a demo ISO path that others can reproduce

## Non-goals for v1.0

- writing a kernel
- replacing init
- pretending SolOS is already a full standalone distro
- collapsing runtime semantics directly into Linux
- implementing every wallet, agent, and app bridge before packaging a demo

## Deliverables

- corrected runtime-core positioning
- architecture docs aligned with the three-layer model
- runtime wording corrected in code and shell copy
- demo appliance folder for ISO assembly
- reproducible documentation for building a bootable SolOS demo image

## Practical statement

If someone asks what SolOS 1.0 is, the short answer is:

**SolOS 1.0 is a Linux-based operating layer that runs above a dedicated runtime intermediary, packaging shell, orchestration, approvals, and agent-native experience into a demo appliance image.**
