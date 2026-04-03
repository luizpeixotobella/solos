# SolOS demo appliance v1

This is the first real scaffold for a **Linux-based SolOS demo image**.

It does **not** pretend to be a finished ISO. Instead, it defines a credible path for turning a small Debian/Ubuntu-style Linux install into a boot-to-SolOS demo appliance:

1. build the existing SolOS shell from `app/shell`
2. serve the built assets locally on `127.0.0.1:8080`
3. auto-login into a lightweight X session
4. launch Chromium in kiosk mode to the local shell

That gives us a plausible v1 demo image architecture without overcommitting to a full distro build system yet.

## Layout

- `bin/` - runtime and provisioning scripts
- `config/systemd/` - user/system services for the demo appliance
- `config/x11/` - X session bootstrap
- `config/autostart/` - desktop-session autostart entry
- `config/environment/` - environment template for overrides
- `scripts/` - helper notes for future image assembly steps

## Intended target

A minimal Linux machine or VM with:

- Debian 12 / Ubuntu 24.04 class base OS
- systemd
- a display manager or TTY autologin path
- Node.js 20+
- Chromium or Google Chrome
- Xorg + Openbox (or another tiny session manager)

## Runtime model

At boot:

- Linux auto-logs in as the `solos` user
- user session starts X
- X session launches the SolOS shell server as a user service
- Chromium opens `http://127.0.0.1:8080` in kiosk mode
- if Chromium exits, the kiosk launcher retries after a short delay

## Quick start on a Linux host

### 1. Build the shell

From repo root:

```bash
cd app/shell
npm install
npm run build
```

### 2. Provision the demo appliance bits

```bash
cd appliance/demo-linux-v1
sudo ./bin/provision-demo-host.sh
```

This script copies configs into the expected locations and prints the remaining manual steps.

### 3. Install the shell into `/opt/solos-shell`

```bash
sudo SOLOS_REPO=/path/to/solos ./bin/install-shell-build.sh
```

### 4. Enable services for the demo user

Log in as `solos` once, then:

```bash
systemctl --user daemon-reload
systemctl --user enable --now solos-shell.service
```

If you use the `.xsession` path, Chromium will be launched from the X session. If you use a desktop environment, copy the autostart file.

## Files of interest

### `bin/provision-demo-host.sh`
Copies service/session config into place and creates the expected directories.

### `bin/install-shell-build.sh`
Copies `app/shell/dist` into `/opt/solos-shell/current` and installs the local static server.

### `bin/run-shell-server.mjs`
A tiny static-file server for production demo use. No external runtime dependency beyond Node.

### `config/x11/solos-xsession.sh`
Minimal X session bootstrap that launches Openbox and the kiosk browser.

### `config/systemd/solos-shell.service`
User service that serves the shell on `127.0.0.1:8080`.

## Suggested demo-image assembly flow

For a future real image build, the likely next step is:

1. start from Debian live-build, Ubuntu autoinstall, or Raspberry Pi OS customization
2. preinstall Node, Chromium, Xorg, Openbox
3. copy this `appliance/demo-linux-v1` tree into the image build context
4. run `provision-demo-host.sh` during image finalization
5. preseed the built shell into `/opt/solos-shell/current`
6. enable autologin for the `solos` user

## Limitations

- Not a full image builder yet
- Assumes Debian/Ubuntu-like paths and package names
- Uses a browser kiosk rather than a native window manager shell
- Does not yet bundle wallet daemons, agent runtime, or OpenClaw pairing UX
- Does not configure graphical autologin automatically because that varies by display manager

## Why this is a good v1

Because it is honest and shippable:

- grounded in the real shell that already exists
- easy to test in a VM
- easy to evolve into a proper appliance image later
- keeps the SolOS product story focused on the shell experience first
