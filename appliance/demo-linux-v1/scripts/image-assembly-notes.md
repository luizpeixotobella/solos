# Future image assembly notes

This directory is intentionally not a fake ISO builder.

For a real SolOS image, likely candidates are:

- Debian `live-build`
- Ubuntu `autoinstall` + postinstall customization
- Raspberry Pi OS image customization flow
- Packer for VM appliance generation

The current appliance scaffold is designed to drop into any of those.

Minimum image steps:

1. create `solos` user
2. enable autologin
3. install Xorg + Openbox + Chromium + Node.js
4. place shell build in `/opt/solos-shell/current`
5. install `run-shell-server.mjs`
6. install user service + X session files
7. set default session to the SolOS kiosk session
