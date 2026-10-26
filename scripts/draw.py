"""Draws images/<keyboard>_<keymap>.svg for each keyboard in qmk.json.

Run by make draw, with keymap-drawer's Python (it imports keymap_drawer).
QMK commands run through $QMK_RUN, the same Docker or native setup that make
build uses.

For each keyboard:
1. Run the keymap through the C preprocessor, so the shared layout macros
   expand to plain keycodes, and convert it to QMK's keymap JSON (c2json).
2. Parse that with keymap-drawer and draw it on the keyboard's physical
   layout: keymap-drawer's own hand-made one if it has one (e.g. the
   Voyager's, with rotated thumb keys), otherwise qmk info's. keymap-drawer's
   usual fallback, QMK's online API, only has upstream QMK's keyboards.
"""

import json
import os
import re
import shlex
import shutil
import subprocess
import sys
from pathlib import Path

from keymap_drawer.physical_layout import QMK_LAYOUTS_PATH

KEYMAP = os.environ.get("KEYMAP", "juliusz")
QMK_RUN = shlex.split(os.environ["QMK_RUN"])
BUILD = Path("build")
IMAGES = Path("images")


def keymap_drawer(*args: str, stdout) -> None:
    subprocess.run(
        # Run as the main module, keymap-drawer's own DeprecationWarnings would show
        [sys.executable, "-W", "ignore::DeprecationWarning", "-m", "keymap_drawer", "-c", "keymap-drawer.yaml", *args],
        stdout=stdout,
        check=True,
    )


def keymaps_dir(keyboard: str) -> Path:
    """The keymap can be in a parent folder, e.g. zsa/moonlander for zsa/moonlander/revb."""
    parts = keyboard.split("/")
    while parts:
        path = Path("keyboards", *parts, "keymaps", KEYMAP)
        if path.is_dir():
            return path
        parts.pop()
    sys.exit(f"No {KEYMAP} keymap for {keyboard}")


def draw(keyboard: str, layer_names: list[str]) -> None:
    name = f"{keyboard.replace('/', '_')}_{KEYMAP}"
    print(f"Drawing {name}", flush=True)

    keymap_c = keymaps_dir(keyboard) / "keymap.c"
    script = f"""
        cpp -P -I users/{KEYMAP} -DQMK_KEYBOARD_H='"/dev/null"' {keymap_c} > {BUILD}/{name}.c
        qmk c2json --no-cpp -q -kb {keyboard} -km {KEYMAP} {BUILD}/{name}.c -o {BUILD}/{name}.json
        qmk info -kb {keyboard} -f json > {BUILD}/{name}.info.json
    """
    subprocess.run([*QMK_RUN, "sh", "-ec", script], check=True)

    layout = json.loads((BUILD / f"{name}.json").read_text())["layout"]
    if (QMK_LAYOUTS_PATH / f"{keyboard.replace('/', '@')}.json").is_file():
        physical_layout = ["-k", keyboard]
    else:
        physical_layout = ["-j", str(BUILD / f"{name}.info.json")]

    with open(BUILD / f"{name}.yaml", "w") as f:
        keymap_drawer("parse", "-q", str(BUILD / f"{name}.json"), "--layer-names", *layer_names, stdout=f)
    with open(IMAGES / f"{name}.svg", "w") as f:
        keymap_drawer("draw", str(BUILD / f"{name}.yaml"), *physical_layout, "-l", layout, stdout=f)


def main() -> None:
    keyboards = [keyboard for keyboard, _ in json.loads(Path("qmk.json").read_text())["build_targets"]]
    # Layer names come from the "#define NAME <number>" lines in layout.h
    layout_h = Path("users", KEYMAP, "layout.h").read_text()
    layer_names = re.findall(r"^#define ([A-Z0-9_]+) [0-9]+$", layout_h, re.MULTILINE)

    # Start from an empty images/, so renamed or removed keyboards don't leave
    # old images behind
    shutil.rmtree(IMAGES, ignore_errors=True)
    IMAGES.mkdir()
    BUILD.mkdir(exist_ok=True)

    for keyboard in keyboards:
        draw(keyboard, layer_names)


if __name__ == "__main__":
    main()
