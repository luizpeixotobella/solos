#!/usr/bin/env bash
set -euo pipefail

repo_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

cd "$repo_dir/app/runtime-core"
cargo fmt --check
cargo test
cargo run --quiet > "$repo_dir/app/shell-native/src/runtime_snapshot.json"

jq -e '
  .schemaVersion == "solos.runtime.snapshot.v1" and
  .productVersion == "1.0.0-rc1" and
  .capabilityManifest.defaultPolicy == "deny" and
  .traceEvaluation.total >= 5 and
  .traceEvaluation.passRate == 1 and
  .quotaProxy.requiresSignedHolderProof == true and
  .walletSession.sponsoredCallsAllowed == false
' "$repo_dir/app/shell-native/src/runtime_snapshot.json" >/dev/null

cd "$repo_dir/app/shell"
npm run build

if command -v cmake >/dev/null 2>&1 && [ -d "$repo_dir/app/shell-native/build" ]; then
  cmake --build "$repo_dir/app/shell-native/build"
fi

bash -n "$repo_dir/appliance/demo-linux-v1/bin/launch-kiosk.sh"
bash -n "$repo_dir/appliance/demo-linux-v1/bin/provision-demo-host.sh"
bash -n "$repo_dir/appliance/demo-linux-v1/live-build/build-iso.sh"

test -s "$repo_dir/docs/demo-v1.0.md"
test -s "$repo_dir/docs/release-v1.0-rc1.md"
test -s "$repo_dir/docs/quota-proxy-v1.md"

echo "SolOS v1.0 RC1 verification passed."
