"""Exercise real local-socket framing, including delayed/partial responses."""
import os
import socket
import subprocess
import sys
import tempfile
import threading
import time

HEALTHY = b'{"ok":true,"result":{"snapshotAvailable":true}}\n'


def check(name, chunks, expected, delay=0, hold=0):
    with tempfile.TemporaryDirectory() as root:
        path = os.path.join(root, 'daemon.sock')
        listener = socket.socket(socket.AF_UNIX)
        listener.bind(path)
        listener.listen(1)
        errors = []

        def serve():
            try:
                with listener.accept()[0] as connection:
                    connection.settimeout(2)
                    request = b''
                    while not request.endswith(b'\n'):
                        request += connection.recv(4096)
                    assert b'health.get' in request
                    for chunk in chunks:
                        connection.sendall(chunk)
                        time.sleep(delay)
                    time.sleep(hold)
            except (BrokenPipeError, ConnectionResetError):
                pass  # Expected for bounded, rejected responses.
            except Exception as error:
                errors.append(error)
            finally:
                listener.close()

        worker = threading.Thread(target=serve, daemon=True)
        worker.start()
        started = time.monotonic()
        result = subprocess.run([sys.argv[1], path], timeout=3)
        elapsed = time.monotonic() - started
        worker.join(timeout=2)
        assert not worker.is_alive(), name + ': server did not finish'
        assert not errors, (name, errors)
        assert result.returncode == expected, (name, result.returncode, expected)
        assert elapsed < 1, (name, 'deadline exceeded', elapsed)
        print(name + ': OK')


check('fragmented health reply', [HEALTHY[:1], HEALTHY[1:]], 0, delay=.06)
check('whole reply', [HEALTHY], 0)
check('missing state', [b'{"ok":true,"result":{"snapshotAvailable":false}}\n'], 1)
check('daemon error', [b'{"ok":false,"result":{"snapshotAvailable":true}}\n'], 1)
check('malformed reply', [b'not-json\n'], 1)
check('unfinished reply', [HEALTHY[:-1]], 1, hold=.4)
check('slow trickle', [b'{', b'"ok"', b':true'], 1, delay=.12)
check('oversized reply', [b' ' * 70000], 1)
check('disconnected', [], 1)
