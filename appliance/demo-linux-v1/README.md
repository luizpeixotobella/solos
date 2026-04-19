# SolOS demo appliance v1.0

This folder contains the first real scaffold for a **Linux-based SolOS demo image / demo ISO**.

The key architectural rule is now explicit:

- Linux is the runtime
- SolOS is the operating layer above Linux
- this appliance packages that relationship into a bootable demo experience

## What this folder is for

Use this folder to assemble a reproducible SolOS v1.0 demo image that:

1. installs on top of a Debian or Ubuntu class Linux base
2. auto-starts the SolOS shell experience
3. uses Linux services and session management underneath
4. can be converted into a VM image or bootable ISO

## v1.0 runtime model

At boot:

1. Linux boots normally
2. system services and session plumbing come from Linux
3. a `solos` user auto-logs in
4. the SolOS shell service starts
5. the SolOS experience becomes the visible operating layer

That means the ISO is not a fake standalone SolOS kernel image. It is a Linux image that boots into the SolOS environment.

## Folder layout

- `bin/` - runtime, provisioning, and install scripts
- `config/systemd/` - user service for SolOS shell runtime
- `config/x11/` - session bootstrap for the demo session
- `config/autostart/` - desktop-session autostart entry
- `config/environment/` - environment template for overrides
- `scripts/` - assembly notes and ISO build helpers

## Current shell modes

This appliance folder can support two SolOS shell modes.

### Mode A, browser kiosk demo
Current lowest-friction path.

- build shell assets
- serve them locally
- launch Chromium in kiosk mode

This is still useful for demos and ISO validation.

### Mode B, native shell migration path
Longer-term path.

- package the native shell as the primary SolOS session
- keep Linux as the runtime substrate
- reduce browser use to app compatibility surfaces

## Fast path, test on a Linux VM

### 1. Build the SolOS shell

From repo root:

```bash
cd app/shell
npm install
npm run build
```

### 2. Provision the demo host files

```bash
cd appliance/demo-linux-v1
sudo ./bin/provision-demo-host.sh
```

### 3. Install the shell build into `/opt/solos-shell`

```bash
sudo SOLOS_REPO=/path/to/solos ./bin/install-shell-build.sh
```

### 4. Enable the service for the demo user

Log in as `solos`, then run:

```bash
systemctl --user daemon-reload
systemctl --user enable --now solos-shell.service
```

### 5. Configure graphical autologin

Use your display manager's normal Linux autologin path for the `solos` user.

### 6. Reboot and validate

On boot, Linux should start first and then enter the SolOS experience automatically.

## How to create a demo ISO

There are multiple viable image-build paths. For v1.0, keep it simple.

### Recommended path: Debian live-build

1. Create a Debian 12 build environment
2. Use `live-build` to generate a live ISO
3. Add packages:
   - xorg
   - openbox
   - chromium
   - nodejs
   - rsync
4. Create the `solos` user during chroot/customization
5. Copy this appliance tree into the image build context
6. Run `bin/provision-demo-host.sh` during image customization
7. Copy `app/shell/dist` into `/opt/solos-shell/current`
8. Enable autologin and the SolOS user service
9. Build the ISO

### Alternative paths

- Ubuntu autoinstall + postinstall customization
- Packer for VM appliance images
- Raspberry Pi OS image customization for ARM demos

## Minimum package set

Suggested packages for the current browser-kiosk demo image:

- systemd
- xorg
- xinit
- openbox
- chromium or google-chrome
- nodejs 20+
- rsync
- ca-certificates
- fonts-dejavu

## Demo ISO expectation

The demo ISO should boot into Linux and quickly hand off the visible experience to SolOS.

That is the correct v1.0 story.

## Files of interest

### `bin/provision-demo-host.sh`
Copies service/session config into place and creates the expected directories.

### `bin/install-shell-build.sh`
Copies the shell build into `/opt/solos-shell/current`.

### `bin/run-shell-server.mjs`
Tiny local server used by the browser-kiosk demo path.

### `config/systemd/solos-shell.service`
User service that serves the shell locally.

### `config/x11/solos-xsession.sh`
Minimal X session bootstrap that launches the SolOS kiosk session.

### `scripts/build-demo-iso.md`
Step-by-step guide for building a reproducible demo ISO.

### `live-build/`
A real Debian `live-build` scaffold with package lists, customization hook, artifact preparation, and ISO build helper scripts.

## Ready-to-use ISO scaffold

A real scaffold now exists under:

```text
appliance/demo-linux-v1/live-build/
```

Typical flow:

```bash
cd appliance/demo-linux-v1/live-build
./prepare-build-context.sh
./build-iso.sh
```

This prepares a Debian live-build context, injects current SolOS artifacts, and builds a hybrid ISO.

## Why this is v1.0-worthy

Because it is honest, packageable, and testable.

It does not claim SolOS replaced Linux.
It shows SolOS acting as the operating layer it is supposed to be.
