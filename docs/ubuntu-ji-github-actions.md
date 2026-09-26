# Build the Ubuntu JI ISO with GitHub Actions

The **Build SolOS Ubuntu JI ISO** workflow is a manual build path that keeps live-build state on a self-hosted runner. It uploads the ISO and SHA-256 checksum as a GitHub Actions artifact; it does not publish a release or deploy an image.

## Runner prerequisites

- A repository-level self-hosted Linux x64 runner with the custom label `solos-iso`.
- At least 25 GB free disk and 8 GB RAM; keep the runner host online while a job runs.
- The runner account must have non-interactive `sudo -n` permission for package installation, build setup, and output ownership restoration.
- Register the runner using GitHub's short-lived token flow and run it as a service. Never commit the registration token.
- Dispatch only from trusted code on `main`; this runner executes build commands with elevated privileges and is intentionally not attached to pull-request workflows.

## Run or resume

1. Open **Actions → Build SolOS Ubuntu JI ISO → Run workflow** on `main`.
2. Choose `--resume` to reuse the persistent staging/cache, or `--fresh` after source or configuration changes.
3. Download the ISO and checksum from the completed run's artifact. Artifacts expire after 14 days; check the repository's storage allowance.

The state directory is `$HOME/.cache/solos-iso/build`, outside the checked-out repository, so checkout cleanup does not erase it. The workflow serializes builds and has a six-hour job limit. If the runner is restarted, dispatch `--resume` again. The full purge mode is intentionally not exposed in the workflow.

## Release status

Adding this workflow does not prove the ISO is valid. Before calling an image releasable, finish one complete workflow run, verify the checksum, boot-test in QEMU and on a spare machine, and complete the signing/SBOM gates in the appliance README.
