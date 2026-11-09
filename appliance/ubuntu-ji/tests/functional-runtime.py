#!/usr/bin/env python3
"""Exercise installed binaries and atomic snapshots, not a fixture daemon."""
import json
import os
from pathlib import Path
import shutil
import socket
import subprocess
import sys
import tempfile
import time

repo = Path(__file__).resolve().parents[3]
binary_dir = Path(sys.argv[1]).resolve()
with tempfile.TemporaryDirectory(prefix="solos-installed-test-") as temporary:
    root = Path(temporary)
    (root / "runtime").mkdir(mode=0o700)
    (root / "release/bin").mkdir(parents=True)
    for name in ("solos-runtime-core", "solos-daemon"):
        shutil.copy2(binary_dir / name, root / "release/bin" / name)
    env = dict(os.environ, SOLOS_SOURCE_DIR=str(repo), SOLOS_RELEASE_DIR=str(root / "release"),
               XDG_RUNTIME_DIR=str(root / "runtime"), XDG_STATE_HOME=str(root / "state"))
    snapshot = root / "runtime/solos/runtime_snapshot.json"
    env["SOLOS_RUNTIME_SNAPSHOT"] = str(snapshot)
    refresh = repo / "appliance/ubuntu-ji/solos-runtime-refresh"
    subprocess.run([refresh], env=env, check=True, timeout=45)
    state = json.loads(snapshot.read_text())
    assert state["hostRuntime"]["hostname"] == socket.gethostname()
    assert state["runtimeMode"] == "runtime-intermediary"
    assert snapshot.stat().st_mode & 0o777 == 0o600
    before = snapshot.read_bytes()
    # A failed publisher must not truncate the last valid state.
    failed = dict(env, SOLOS_RELEASE_DIR=str(root / "missing"))
    assert subprocess.run([refresh], env=failed, capture_output=True).returncode != 0
    assert snapshot.read_bytes() == before
    sockpath = root / "runtime/solos/daemon.sock"
    with open(root / "daemon.log", "w") as log:
        daemon = subprocess.Popen([root / "release/bin/solos-daemon", "serve"], env=env, stdout=log, stderr=log)
        try:
            for _ in range(100):
                if sockpath.exists(): break
                if daemon.poll() is not None: raise AssertionError((root / "daemon.log").read_text())
                time.sleep(.05)
            with socket.socket(socket.AF_UNIX) as client:
                client.settimeout(5)
                client.connect(str(sockpath))
                stream = client.makefile("rwb")
                stream.write(b'{"id":"installed-test","method":"snapshot.get"}\n'); stream.flush()
                response = json.loads(stream.readline())
                assert response["ok"], response
                assert response["result"]["hostRuntime"]["hostname"] == socket.gethostname()
                assert response["result"]["sessionLabel"] == state["sessionLabel"]
            if len(sys.argv) > 2:
                qt_env = dict(env, SOLOS_DAEMON_SOCKET=str(sockpath))
                subprocess.run([str(Path(sys.argv[2]).resolve())], env=qt_env, check=True, timeout=10)
            assert sockpath.stat().st_mode & 0o777 == 0o600
        finally:
            daemon.terminate()
            daemon.wait(timeout=5)
    print("PASS actual installed runtime → atomic private state → actual daemon → snapshot.get; failed refresh preserves previous state")
