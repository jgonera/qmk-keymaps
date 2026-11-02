// Settings shared by all keyboards.

#pragma once

#define PERMISSIVE_HOLD
#define LAYER_STATE_8BIT

// Starting brightness of the key lights, 0-255, until it's changed with
// LIGHT_DN/LIGHT_UP (see juliusz.c)
#define KEY_LIGHT_DEFAULT_LEVEL 16

// How long FLASH has to be held to restart into the bootloader
#define FLASH_HOLD_MS 1000
