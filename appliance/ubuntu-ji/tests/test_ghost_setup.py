import importlib.machinery
import importlib.util
import json
from pathlib import Path
import tempfile
import unittest
import urllib.error
from unittest.mock import patch

loader = importlib.machinery.SourceFileLoader('setup', str(Path(__file__).parents[1] / 'solos-ghost-setup'))
spec = importlib.util.spec_from_loader('setup', loader)
setup = importlib.util.module_from_spec(spec)
loader.exec_module(setup)


class LocalSetupTests(unittest.TestCase):
    def test_discrete_not_integrated(self):
        self.assertEqual(setup.choose_device(' Vulkan0: Intel UHD\n Vulkan1: NVIDIA GTX 1050 Ti'), 'Vulkan1')

    def test_amd_supported(self):
        self.assertEqual(setup.choose_device(' Vulkan0: AMD Radeon RX 6600'), 'Vulkan0')

    def test_virtual_gpu_not_accepted(self):
        with self.assertRaises(RuntimeError):
            setup.choose_device(' Vulkan0: llvmpipe\n Vulkan1: Intel UHD')

    def test_explicit_cpu(self):
        self.assertEqual(setup.choose_device('', cpu=True), 'none')

    def test_private_atomic_and_symlink(self):
        with tempfile.TemporaryDirectory() as td:
            p = Path(td) / 'config' / 'key'
            setup.atomic(p, 'synthetic-test-key')
            self.assertEqual(p.stat().st_mode & 0o777, 0o600)
            self.assertEqual(p.parent.stat().st_mode & 0o777, 0o700)
            link = p.parent / 'link'
            link.symlink_to(p)
            with self.assertRaises(RuntimeError):
                setup.atomic(link, 'replacement')
            self.assertEqual(p.read_text(), 'synthetic-test-key')

    def test_manual_and_disabled_profile_preserved(self):
        with tempfile.TemporaryDirectory() as td, patch.object(setup, 'CONFIG', Path(td)):
            p = Path(td) / 'ghost-api.json'
            for profile in ({'schema': 'solos.ghost.client.v1', 'key': 'private'},
                            {'managedBy': 'solos-ghost-setup', 'disabled': True}):
                p.write_text(json.dumps(profile))
                with self.assertRaises(RuntimeError):
                    setup.check_profile()
                self.assertEqual(json.loads(p.read_text()), profile)

    def test_hash_failure_does_not_replace_artifact(self):
        with tempfile.TemporaryDirectory() as td:
            p = Path(td) / 'model'
            p.write_bytes(b'original')
            with patch.object(setup.urllib.request, 'urlopen') as network:
                with self.assertRaises(RuntimeError):
                    setup.download('https://invalid.test', p, 'bad-hash')
                network.assert_not_called()
            self.assertEqual(p.read_bytes(), b'original')

    def test_real_generation_and_auth_required(self):
        unauthorized = urllib.error.HTTPError('test', 401, 'Unauthorized', {}, None)
        result = {'choices': [{'message': {'content': 'GHOST-GPU-OK'}}], 'usage': {'completion_tokens': 8}}
        with patch.object(setup, 'request', side_effect=[unauthorized, result]) as request:
            proof = setup.probe('test-secret')
        self.assertEqual(proof['marker'], 'GHOST-GPU-OK')
        self.assertNotIn('test-secret', json.dumps(proof))
        self.assertEqual(request.call_count, 2)

    def test_unauthenticated_listener_rejected(self):
        with patch.object(setup, 'request', return_value={}):
            with self.assertRaises(RuntimeError):
                setup.probe('test-secret')

    def test_wrong_answer_not_promoted(self):
        unauthorized = urllib.error.HTTPError('test', 401, 'Unauthorized', {}, None)
        result = {'choices': [{'message': {'content': 'unrelated'}}]}
        with patch.object(setup, 'request', side_effect=[unauthorized, result]):
            with self.assertRaises(RuntimeError):
                setup.probe('test-secret')


if __name__ == '__main__':
    unittest.main()
