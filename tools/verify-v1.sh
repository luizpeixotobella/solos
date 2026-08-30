#!/usr/bin/env bash
set -euo pipefail

repo_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
generated_snapshot="$(mktemp /tmp/solos-runtime-snapshot.XXXXXX.json)"
trap 'rm -f "$generated_snapshot"' EXIT

cd "$repo_dir/app/runtime-core"
cargo fmt --check
cargo test
cargo run --quiet > "$generated_snapshot"
cargo test --bin solos-daemon
cargo build --bin solos-daemon --bin ghost-audit-verify

jq -e '
  .schemaVersion == "solos.runtime.snapshot.v1" and
  .productVersion == "1.0.0-rc2" and
  .capabilityManifest.defaultPolicy == "deny" and
  .traceEvaluation.total >= 5 and
  .traceEvaluation.passRate == 1 and
  .ghost.resolutionLoop.schema == "solos.ghost.resolutions.v1" and
  .ghost.resolutionLoop.selectedId == "resolution-safe-workspace" and
  .ghost.auditChallenge.schema == "solos.ghost.audits.v1" and
  .ghost.auditChallenge.audits == [] and
  (.capabilityManifest.capabilities | any(.id == "ghost.audit.proof.write" and .executable == true)) and
  (.capabilityManifest.capabilities | any(.id == "ghost.audit.proof.verify" and .executable == true)) and
  .quotaProxy.requiresSignedHolderProof == true and
  .walletSession.sponsoredCallsAllowed == false
' "$generated_snapshot" >/dev/null

cd "$repo_dir/app/shell"
npm run build

SOLOS_SMOKE_SNAPSHOT="$generated_snapshot" "$repo_dir/tools/smoke-ghost-resolution.sh"
SOLOS_SMOKE_SNAPSHOT="$generated_snapshot" "$repo_dir/tools/smoke-ghost-audit.sh"

if command -v curl >/dev/null 2>&1; then
  "$repo_dir/tools/smoke-web-shell.sh"
fi

if command -v cmake >/dev/null 2>&1 && [ -d "$repo_dir/app/shell-native/build" ]; then
  cmake --build "$repo_dir/app/shell-native/build"
  SOLOS_SMOKE_SNAPSHOT="$generated_snapshot" "$repo_dir/tools/smoke-native-shell.sh"
fi

bash -n "$repo_dir/appliance/demo-linux-v1/bin/launch-kiosk.sh"
bash -n "$repo_dir/appliance/demo-linux-v1/bin/provision-demo-host.sh"
bash -n "$repo_dir/appliance/demo-linux-v1/bin/install-daemon-build.sh"
bash -n "$repo_dir/appliance/demo-linux-v1/live-build/build-iso.sh"
bash -n "$repo_dir/tools/smoke-web-shell.sh"
bash -n "$repo_dir/tools/smoke-native-shell.sh"
bash -n "$repo_dir/tools/smoke-ghost-resolution.sh"
bash -n "$repo_dir/tools/smoke-ghost-audit.sh"
bash -n "$repo_dir/tools/ghost-audit-pilot.sh"
node --check "$repo_dir/tools/sync-ghost-brain.mjs"
test -s "$repo_dir/appliance/demo-linux-v1/config/systemd/solos-ghost-sync.service"
test -s "$repo_dir/appliance/demo-linux-v1/config/systemd/solos-ghost-sync.timer"
test -s "$repo_dir/appliance/demo-linux-v1/config/systemd-user/solos-daemon.service"
test -s "$repo_dir/appliance/demo-linux-v1/config/systemd-user/solos-ghost-sync.service"
test -s "$repo_dir/appliance/demo-linux-v1/config/systemd-user/solos-ghost-sync.timer"
bash -n "$repo_dir/appliance/demo-linux-v1/bin/install-user-runtime.sh"

test -s "$repo_dir/docs/demo-v1.0.md"
test -s "$repo_dir/docs/release-v1.0-rc1.md"
test -s "$repo_dir/docs/release-v1.0-rc2.md"
test -s "$repo_dir/docs/quota-proxy-v1.md"
test -s "$repo_dir/docs/daemon-v1.md"
test -s "$repo_dir/docs/ghost-resolution-loop.md"
test -s "$repo_dir/docs/ghost-audit-pilot.md"
test -s "$repo_dir/docs/ghost-audit-pilot-campaign.md"
test -s "$repo_dir/.github/ISSUE_TEMPLATE/ghost-audit-pilot.yml"
jq -e '
  .schema == "solos.ghost.audit.pilot.v1" and
  .cohortTarget == 10 and
  .validReviewerReturns == 0 and
  .acquisitionBaseline.totalClones == 24 and
  .acquisitionBaseline.uniqueCloners == 17
' "$repo_dir/data/ghost-audit-pilot.json" >/dev/null

echo "SolOS v1.0 RC2 verification passed."
