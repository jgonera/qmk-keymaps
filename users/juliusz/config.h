// Settings shared by all keyboards.

#pragma once

#define PERMISSIVE_HOLD
// For the Shift keys on Z and / only, see get_hold_on_other_key_press in
// juliusz.c
#define HOLD_ON_OTHER_KEY_PRESS_PER_KEY
#define LAYER_STATE_8BIT

// How long FLASH_KEY has to be held to restart into the bootloader
#define FLASH_HOLD_MS 3000

// GUI_BSPC held alone this long switches from Gui to repeating Backspace,
// one every GUI_BSPC_REPEAT_INTERVAL_MS (30 matches macOS's fastest key
// repeat, KeyRepeat 2)
#define GUI_BSPC_REPEAT_DELAY_MS 500
#define GUI_BSPC_REPEAT_INTERVAL_MS 30
