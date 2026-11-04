#!/usr/bin/env python3
"""Create a NEW optional ISO overlay; never install or merge into a running host."""
import pathlib
import shutil
import sys

source = pathlib.Path(__file__).resolve().parent.parent
if len(sys.argv) != 2:
    raise SystemExit('usage: stage-iso-overlay.py NEW_STAGING_DIRECTORY')
dest = pathlib.Path(sys.argv[1]).expanduser().resolve()
if dest.exists():
    raise SystemExit('Destination must be new; refusing to overwrite or install on host.')
if not (source / 'node_modules/hardhat/package.json').is_file():
    raise SystemExit('Run npm ci --ignore-scripts in the lab on the target Linux architecture first.')
if source in dest.parents or dest in source.parents:
    raise SystemExit('Destination cannot contain or be inside the source.')

lab = dest / 'opt/solos/labs/heart-pass'
# Only our generated receipts are excluded. Safe's node_modules/build/artifacts
# are essential deployment bytecode and MUST remain in the offline overlay.
def ignore_generated(directory, names):
    return ['artifacts'] if pathlib.Path(directory).resolve() == source else []

shutil.copytree(source, lab, ignore=ignore_generated)
launcher = dest / 'usr/local/bin/solos-heart-pass-lab'
launcher.parent.mkdir(parents=True)
shutil.copyfile(source / 'scripts/launch-lab.sh', launcher)
launcher.chmod(0o755)
desktop = dest / 'usr/share/applications/solos-heart-pass-lab.desktop'
desktop.parent.mkdir(parents=True)
desktop.write_text('''[Desktop Entry]
Type=Application
Name=Heart Pass Lab
Comment=Protótipo local sem dinheiro real e sem carteira pessoal
Exec=/usr/local/bin/solos-heart-pass-lab
Terminal=true
Categories=Development;
Icon=applications-development
''')
print(f'Overlay created: {dest}')
print('Not installed. No service/autostart changes. VM/ISO integration remains unverified.')
