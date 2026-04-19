# SolOS v1.0

## Definition

SolOS v1.0 is the first explicit version of SolOS as a **Linux-based operating layer**.

It is not a new kernel and not a replacement runtime.

It is:
- a shell experience
- an orchestration layer
- an agent-aware environment
- an approval and ownership surface
- a packageable Linux appliance experience

## Version rule

For v1.0, the runtime comes from Linux.

SolOS sits above Linux and turns host runtime state into:
- shell state
- system awareness
- app and capability visibility
- approvals
- agent-facing orchestration

## Components in v1.0

### 1. Linux host runtime
The underlying runtime, services, session model, networking, and filesystem all come from Linux.

### 2. SolOS runtime adapter
`app/runtime-core` reads Linux host state and emits structured SolOS-visible runtime data.

### 3. SolOS shell
The shell presents Home, Agent, Wallet, and Apps as one coherent operating layer.

### 4. Appliance packaging
`appliance/demo-linux-v1` contains the first credible path to a demo image/ISO.

## What changed in this correction

Before this correction, the Rust runtime-core still looked too much like a static, self-contained runtime generator.

Now the v1.0 direction is:
- Linux runtime first
- Rust as adapter/orchestrator
- SolOS as operating layer
- image packaging around Linux instead of around an invented substrate

## v1.0 goals

- be architecturally honest
- boot credibly on Linux
- expose a coherent shell
- document the build and image story clearly
- create a demo ISO path that others can reproduce

## Non-goals for v1.0

- writing a kernel
- replacing init
- pretending SolOS is already a full standalone distro
- implementing every wallet, agent, and app bridge before packaging a demo

## Deliverables

- corrected runtime-core positioning
- architecture docs aligned with Linux-host runtime
- demo appliance folder for ISO assembly
- reproducible documentation for building a bootable SolOS demo image

## Practical statement

If someone asks what SolOS 1.0 is, the short answer is:

**SolOS 1.0 is a Linux-based operating layer that launches a SolOS shell on top of the Linux runtime and packages that experience as a demo appliance image.**
