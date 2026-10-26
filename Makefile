# Builds firmware and draws images for all keyboards listed in qmk.json.
# See README.md.

# ZSA's fork of QMK, which supports the Moonlander rev B (Ignition bootloader)
# that upstream QMK doesn't. QMK_COMMIT is pinned to the head of QMK_BRANCH.
QMK_REPO := https://github.com/zsa/qmk_firmware.git
QMK_BRANCH := firmware25
QMK_COMMIT := 93b2b9ec3368f86c5eb5a2e3f934049f8daef885
QMK_IMAGE := ghcr.io/qmk/qmk_cli:1.2.0
KEYMAP_DRAWER_VERSION := 0.23.0

SHELL := bash

# A disposable cache, cloned by the setup target when needed and deleted by
# make clean.
QMK_FIRMWARE ?= $(HOME)/.cache/qmk_firmware
KEYMAP_DRAWER_PYTHON ?= uvx --from keymap-drawer==$(KEYMAP_DRAWER_VERSION) python

KEYMAP := juliusz
KEYBOARDS := $(shell python3 -c 'import json; print(*(kb for kb, _ in json.load(open("qmk.json"))["build_targets"]))')

# Use a native QMK install if there is one (e.g. in CI), Docker otherwise.
ifeq ($(shell command -v qmk 2>/dev/null),)
QMK_RUN := docker run --rm --init \
	-v "$(QMK_FIRMWARE)":/qmk_firmware -v "$(CURDIR)":/qmk_userspace \
	-e QMK_HOME=/qmk_firmware -e QMK_USERSPACE=/qmk_userspace \
	-w /qmk_userspace $(QMK_IMAGE)
else
QMK_RUN := env QMK_HOME="$(QMK_FIRMWARE)" QMK_USERSPACE="$(CURDIR)"
endif

REPO := jgonera/qmk-keymaps
ZAPP ?= zapp

.PHONY: all setup build draw flash clean

all: build draw

# Checks out QMK_COMMIT of QMK_REPO in QMK_FIRMWARE, cloning it if needed.
# build and draw run it automatically.
setup:
	@set -e; \
	git_qmk() { git -C "$(QMK_FIRMWARE)" "$$@"; }; \
	if [ ! -d "$(QMK_FIRMWARE)" ]; then \
		git init -q "$(QMK_FIRMWARE)"; \
		git_qmk remote add origin $(QMK_REPO); \
	fi; \
	if [ "$$(git_qmk rev-parse HEAD 2>/dev/null)" != "$(QMK_COMMIT)" ]; then \
		echo "Checking out $(QMK_REPO) $(QMK_BRANCH) at $(QMK_COMMIT)"; \
		git_qmk remote set-url origin $(QMK_REPO); \
		git_qmk fetch -q --depth 1 origin $(QMK_COMMIT); \
		git_qmk -c advice.detachedHead=false checkout -q --force FETCH_HEAD; \
		git_qmk submodule -q sync --recursive; \
		git_qmk submodule -q update --init --recursive --depth 1 --force; \
	fi

build: setup
	$(QMK_RUN) qmk userspace-compile
	rm -rf build/firmware
	mkdir -p build/firmware
	mv -f *.hex *.bin build/firmware/

# Draws images/, named like the firmware files (e.g. zsa_voyager_juliusz.svg).
# See scripts/draw.py.
draw: setup
	@QMK_RUN='$(QMK_RUN)' KEYMAP=$(KEYMAP) $(KEYMAP_DRAWER_PYTHON) scripts/draw.py

# Flashes a keyboard with zapp, e.g. make flash KB=moonlander. KB can be any
# unique part of a keyboard name in qmk.json. Uses the local build if there is
# one, otherwise downloads the firmware from the latest release, so it doesn't
# need QMK or Docker.
flash:
	@set -e; \
	matches=(); \
	for qmk_kb in $(KEYBOARDS); do \
		[[ -n "$(KB)" && $$qmk_kb == *"$(KB)"* ]] && matches+=("$$qmk_kb"); \
	done; \
	if [ $${#matches[@]} -ne 1 ]; then \
		echo "Usage: make flash KB=<keyboard>, where <keyboard> matches one of: $(KEYBOARDS)" >&2; \
		exit 1; \
	fi; \
	command -v "$(ZAPP)" >/dev/null || { \
		echo "zapp not found. Install it (brew install zapp), or download it from" >&2; \
		echo "https://github.com/zsa/zapp/releases and pass ZAPP=/path/to/zapp." >&2; \
		exit 1; \
	}; \
	name=$${matches[0]//\//_}_$(KEYMAP); \
	file=$$(ls build/firmware/$$name.* 2>/dev/null || true); \
	if [ -n "$$file" ]; then \
		echo "Flashing $$file (local build from $$(date -u -r "$$file" '+%Y-%m-%d %H:%M UTC'))"; \
	else \
		echo "No local build in build/firmware/ (run 'make build' to build one), using the latest release"; \
		tag=$$(curl -fsSLI -o /dev/null -w '%{url_effective}' https://github.com/$(REPO)/releases/latest); \
		tag=$${tag##*/}; \
		mkdir -p build/release; \
		for ext in bin hex; do \
			url=https://github.com/$(REPO)/releases/download/$$tag/$$name.$$ext; \
			if curl -fsSL -o build/release/$$name.$$ext "$$url" 2>/dev/null; then \
				file=build/release/$$name.$$ext; \
				break; \
			fi; \
		done; \
		[ -n "$$file" ] || { echo "No $$name firmware in release $$tag" >&2; exit 1; }; \
		echo "Flashing $$file from release $$tag"; \
	fi; \
	"$(ZAPP)" flash "$$file"

# Also deletes QMK_FIRMWARE, so don't point it at a checkout you care about.
clean:
	rm -rf build "$(QMK_FIRMWARE)"
