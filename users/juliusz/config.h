// Settings shared by all keyboards.

#pragma once

#define PERMISSIVE_HOLD
#define LAYER_STATE_8BIT

// Starting brightness of the key lights, 0-255, until it's changed with
// LIGHT_DN/LIGHT_UP (see juliusz.c)
#define KEY_LIGHT_DEFAULT_LEVEL 16

// How long FLASH has to be held to restart into the bootloader
#define FLASH_HOLD_MS 1000

// GUI_BSPC held alone this long switches from Gui to repeating Backspace,
// one every GUI_BSPC_REPEAT_INTERVAL_MS (30 matches macOS's fastest key
// repeat, KeyRepeat 2)
#define GUI_BSPC_REPEAT_MS 500
#define GUI_BSPC_REPEAT_INTERVAL_MS 30
