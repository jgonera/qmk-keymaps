# Notes for agents

QMK external userspace built with ZSA's fork (`QMK_REPO`/`QMK_COMMIT` in the
`Makefile`). Targets are in `qmk.json`. README.md covers usage.

## Files

- `users/juliusz/layout.h`: the layers, written in the shape of the ErgoDox
  (`LAYOUT_ergodox_pretty` order), plus custom keycodes. Most layout changes
  only touch this file.
- `users/juliusz/wrappers.h`: maps those layers onto each keyboard's `LAYOUT`.
- `users/juliusz/juliusz.c`, `config.h`, `rules.mk`: shared code and settings.
- `keyboards/zsa/*/keymaps/juliusz/`: per-keyboard keymaps. Each `keymap.json`
  enables ZSA's `zsa/defaults` module, which ZSA's keyboard code needs to
  compile (don't remove it; `zsa/oryx` is deliberately not enabled).
- `scripts/draw.py`, `keymap-drawer.yaml`: image generation (`make draw`).

## Workflow

- `make` builds the firmware and redraws `images/`. Builds run in Docker, so
  Docker Desktop must be running. Regenerated images must be committed with
  the layout change: CI fails if they're out of date.
- Check a layout change by looking at the regenerated images (e.g. render
  with `qlmanage -t -s 1000 -o <dir> images/<file>.svg`).
- Never flash a keyboard yourself; flashing needs the user to press reset.
- Changes go through a branch and a PR; merging to `main` publishes a release
  that `make flash` downloads.

## Gotchas

- `layout.h` is column-aligned (18-character cells). Keep the alignment and
  the line-ending backslashes when editing.
- Layer numbers are `#define`s, not an enum: `make draw` reads them for layer
  names and needs numbers in `LT()`.
- New custom keycodes (`QK_USER_n`) need a label in `keymap-drawer.yaml`'s
  `raw_binding_map`, keyed by the expanded name (e.g. `QK_USER_0`).
- The ErgoDox's flash is small (32 KB); `qmk compile` prints its usage.
- Avoid generic names for `#define`s: e.g. `FLASH` clashes with the STM32
  headers on the Moonlander and Voyager.
- The Moonlander and Voyager builds always show `[WARNINGS]`: it's a harmless
  LTO note (`using serial compilation of 2 LTRANS jobs`).
