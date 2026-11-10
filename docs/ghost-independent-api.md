# Ghost extracted into an independent service

Date: 2026-10-25. Source implementation: `luizpeixotobella/ghost`, release 0.1.0.

Luiz requested Ghost usable both by SolOS and as an everyday OpenClaw model. The original Ghost policy core now has its own Rust crate. SolOS vendors that crate for reproducible offline ISO builds and preserves all old module names with compatibility re-exports. Linux execution, the Daemon, approvals and Wallet remain SolOS responsibilities.

The native shell's **Agent → Talk to Ghost** card uses `GhostApiClient` (Qt Network), not browser JavaScript or a shell command. Configure your self-hosted base URL, API key and model ID in that card. HTTPS is required except on loopback; redirects are never followed with credentials. There is no hosted-provider default or fallback. Saving a setup is not a connection or inference test: only sending a message contacts the configured service. Model output never executes as a host action.

Setup is stored atomically in `$XDG_CONFIG_HOME/solos/ghost-api.json` (normally `~/.config/solos/ghost-api.json`), file 0600 inside a 0700 directory. This is owner-only local file storage, not an encrypted vault. Keys are password-masked on input, cleared from the field after saving, and never exposed as a QML property or echoed in status/errors. A blank key retains the current credential only for an unchanged endpoint. Saving clears conversation history and cancels any in-flight reply so history cannot cross endpoints/models.

**Remove saved setup** atomically replaces credentials with a key-free disabled marker, clears session state and cancels in-flight requests. The marker prevents old environment credentials silently reconnecting at restart. It does not delete an independently managed environment key file. **Clear conversation** cancels a pending message and clears session history without removing configuration. History remains session-only and bounded to ten exchanges; failed requests do not append their prompts. Remote error bodies are not displayed or logged. Responses are limited to 2 MiB and a total three-minute deadline.

For compatibility only, when no saved configuration exists, an explicit `SOLOS_GHOST_API_URL` and/or private `SOLOS_GHOST_API_KEY_FILE` can configure the client. `SOLOS_GHOST_MODEL` defaults to `ghost-ji`. If only a key-file path is supplied, the legacy loopback base is `http://127.0.0.1:18791/v1`. Environment setup is never persisted automatically; saved/disabled/invalid local configuration takes precedence. No key belongs in source, sample data or the ISO.

Documented deployment addresses (not probed in this source stage) are `http://127.0.0.1:18791/v1` or an owner's private tunnel such as `https://YOUR-GHOST-HOST.your-tailnet.ts.net:8443/v1`. Model ID: `ghost-ji`. OpenClaw provider reference: `ghost/ghost-ji`, alias `Ghost`. A historical October deployment used an OpenClaw-hosted reasoning backend; this client change neither selects nor verifies the backend behind an owner's endpoint. It introduces no OpenAI/provider fallback or claim of newly trained weights. Current actual self-hosted inference requires separate authorized endpoint/access evidence.

Local verification: build with `BUILD_TESTING=ON`, then `ctest --test-dir <native-build> --output-on-failure`. `ghost-local-config` uses a controlled loopback HTTP mock and synthetic credentials to exercise configuration, persistence/privacy, explicit model/auth routing, errors, redirect refusal and cancellation. These tests are **not** real inference. The existing `ghost-api-smoke --live` remains a separate opt-in integration check and must never be run with credentials in an offline guest boot test.

An already installed SolOS or an older ISO does not receive these changes automatically. Build/install this shell on the target device and provision credentials privately. Full ISO build and VM boot were not implied by an API deployment.

Rollback: reinstall the previous shell or revert this integration commit. Existing state schemas and daemon capability boundaries are preserved. Vendor updates must come from a reviewed independent Ghost version, not divergent edits in this repository. See the Ghost repository's operations guide for service rollback and future standalone/local inference options.

Co-authorship: Luiz Carlos Peixoto Bella (founder and human product direction), and Luigi, LBArtes artificial intelligence (technical and creative co-author).
