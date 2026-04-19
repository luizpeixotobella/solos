# Build a SolOS v1.0 demo ISO

This guide explains how to assemble a **demo ISO** where:

- Linux provides the runtime
- SolOS provides the shell and operating layer
- the machine boots directly into the SolOS demo experience

## Goal

Produce a bootable demo ISO for SolOS v1.0 using a Debian-class Linux base.

## Recommended base

Use Debian 12 with `live-build`.

Why:
- stable
- well documented
- good fit for kiosk/live images
- easy to customize for a first reproducible SolOS demo

## Architecture assumption

The ISO architecture is:

1. Linux kernel + Debian userspace
2. systemd and standard Linux session plumbing
3. auto-login user `solos`
4. SolOS service and session bootstrap
5. SolOS shell as the visible environment

## Prerequisites on the build machine

Install:

```bash
sudo apt update
sudo apt install -y live-build rsync git nodejs npm
```

## Step 1, build the SolOS shell assets

From the repository root:

```bash
cd app/shell
npm install
npm run build
cd ../..
```

This should create:

```text
app/shell/dist
```

## Step 2, create a live-build workspace

Example:

```bash
mkdir -p /tmp/solos-iso
cd /tmp/solos-iso
lb config \
  --distribution bookworm \
  --debian-installer live \
  --architectures amd64 \
  --binary-images iso-hybrid
```

## Step 3, add required packages

Create:

```text
config/package-lists/solos.list.chroot
```

With:

```text
systemd
xorg
xinit
openbox
chromium
nodejs
npm
rsync
ca-certificates
fonts-dejavu
sudo
```

If `chromium` is unavailable in your environment, substitute the browser package available in your repo.

## Step 4, copy SolOS appliance assets into the chroot hooks

Create a hook, for example:

```text
config/hooks/live/0100-solos-appliance.chroot
```

Make it executable and use it to:

- create the `solos` user
- copy `appliance/demo-linux-v1` files into the image
- copy built shell assets into `/opt/solos-shell/current`
- run provisioning

Example script:

```bash
#!/bin/bash
set -e

useradd -m -s /bin/bash solos || true
mkdir -p /opt/solos-shell/current /opt/solos-shell/bin
mkdir -p /home/solos/.config/systemd/user /home/solos/.config/autostart /home/solos/.config/solos

cp -r /build/solos/appliance/demo-linux-v1 /opt/solos-appliance
cp -r /build/solos/app/shell/dist/. /opt/solos-shell/current/
cp /opt/solos-appliance/bin/run-shell-server.mjs /opt/solos-shell/bin/run-shell-server.mjs

cd /opt/solos-appliance
SOL_USER=solos ./bin/provision-demo-host.sh || true
chown -R solos:solos /home/solos /opt/solos-shell
```
```

Adjust paths to match however you inject the repo into the live-build context.

## Step 5, enable autologin

Autologin varies by display manager.

For v1.0, choose one simple path and document it in your image build:

- LightDM autologin, or
- `getty` autologin + `startx`

The requirement is only this:

- Linux logs into user `solos`
- the session starts the SolOS shell automatically

## Step 6, enable the user service

During customization or first login provisioning, run:

```bash
systemctl --global enable default.target
```

And for the `solos` user environment, ensure:

```bash
systemctl --user daemon-reload
systemctl --user enable solos-shell.service
```

Depending on image strategy, you may need to seed user-systemd state differently.

## Step 7, build the ISO

Run:

```bash
sudo lb build
```

Expected output:

```text
live-image-amd64.hybrid.iso
```

## Step 8, test in a VM

Use QEMU, GNOME Boxes, VirtualBox, or VMware.

Validation checklist:

- ISO boots
- Linux session starts normally
- user `solos` auto-logs in
- local SolOS shell service starts
- browser opens the SolOS shell
- reboot returns to the SolOS environment reliably

## What to improve after the first ISO

After the first demo ISO works, the next improvements should be:

- replace browser kiosk with native shell session
- connect approvals to real Linux-backed actions
- connect app registry to host-discovered apps
- connect agent and wallet bridges to real services
- add branding, installer polish, and update flow

## Honest description for the demo

When presenting this ISO, describe it like this:

**This is SolOS v1.0 packaged as a Linux-based operating layer demo image. Linux provides the runtime. SolOS provides the shell, orchestration, approvals, and agent-native experience.**
