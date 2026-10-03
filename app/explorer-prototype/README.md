# SolOS Explorer Prototype

An isolated Qt/QML desktop Explorer prototype. It leaves the existing native shell untouched and provides a personalized home view, quick access to common folders, a local file browser, and launchers for installed desktop applications.

On startup it asks the current user's systemd manager to start `solos-daemon.service` and checks the configured SolOS daemon socket. It does not install or modify the service. The service unit must already be installed and enabled/configured for the runtime to become available.

The Ubuntu JI ISO build compiles this application into `/opt/solos/current/bin/solos-explorer-prototype` and adds **SolOS Explorer** to the Applications menu. It launches maximized; the existing default SolOS kiosk session remains unchanged.

Build with Qt 6.4 or newer:

```sh
cmake -S app/explorer-prototype -B /tmp/solos-explorer-build
cmake --build /tmp/solos-explorer-build
```

Run `/tmp/solos-explorer-build/solos-explorer-prototype` from a desktop session. Prototype only; this does not replace the SolOS native shell or alter the appliance launcher.
