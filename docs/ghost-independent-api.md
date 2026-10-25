# Ghost extracted into an independent service

Date: 2026-10-25. Source implementation: `luizpeixotobella/ghost`, release 0.1.0.

Luiz requested Ghost usable both by SolOS and as an everyday OpenClaw model. The original Ghost policy core now has its own Rust crate. SolOS vendors that crate for reproducible offline ISO builds and preserves all old module names with compatibility re-exports. Linux execution, the Daemon, approvals and Wallet remain SolOS responsibilities.

The native shell's **Agent → Talk to Ghost** card uses `GhostApiClient` (Qt Network), not browser JavaScript or a shell command. Set `SOLOS_GHOST_API_URL` and a protected `SOLOS_GHOST_API_KEY_FILE` in the shell process environment. HTTPS is required except on loopback; redirects are not followed with credentials. No key belongs in source, sample data or the ISO. The chat has bounded session-only history, error/retry handling and a clear button. It never auto-executes model output.

The host service is available locally at `http://127.0.0.1:18791/v1` and privately at `https://YOUR-GHOST-HOST.your-tailnet.ts.net:8443/v1` for connected Tailscale devices. Model ID: `ghost-ji`. OpenClaw provider reference: `ghost/ghost-ji`, alias `Ghost`. The initial reasoning backend is the already-authorized OpenClaw `openai/gpt-6.1-sol` model; no claim of a newly trained model or proven superintelligence is made.

An already installed SolOS or an older ISO does not receive these changes automatically. Build/install this shell on the target device and provision credentials privately. Full ISO build and VM boot were not implied by an API deployment.

Rollback: reinstall the previous shell or revert this integration commit. Existing state schemas and daemon capability boundaries are preserved. Vendor updates must come from a reviewed independent Ghost version, not divergent edits in this repository. See the Ghost repository's operations guide for service rollback and future standalone/local inference options.

Co-authorship: Luiz Carlos Peixoto Bella (founder and human product direction), and Luigi, LBArtes artificial intelligence (technical and creative co-author).
