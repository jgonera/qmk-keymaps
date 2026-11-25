// Custom key behavior shared by all keyboards: Esc/Caps Lock, Alt/< and
// Gui/>, FLASH_KEY and its light (see layout.h for the keycodes).

#include QMK_KEYBOARD_H
#include "layout.h"

#ifdef RGB_MATRIX_ENABLE
// Key lights (Moonlander, Voyager): all off, except FLASH_KEY, which is red at
// full brightness while its layer is active.

void keyboard_post_init_user(void) {
    // The lights are drawn by rgb_matrix_indicators_advanced_user below, on
    // top of a black solid color. This is saved in EEPROM (only written if
    // it changed), so it also survives the right half of a Moonlander being
    // reconnected, which reloads the RGB settings from EEPROM.
    if (!rgb_matrix_is_enabled()) rgb_matrix_enable();
    if (rgb_matrix_get_mode() != RGB_MATRIX_SOLID_COLOR) rgb_matrix_mode(RGB_MATRIX_SOLID_COLOR);
    if (rgb_matrix_get_val() != 0) rgb_matrix_sethsv(HSV_BLACK);
}

bool rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) {
    for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
        for (uint8_t col = 0; col < MATRIX_COLS; col++) {
            uint8_t led = g_led_config.matrix_co[row][col];
            if (led == NO_LED || led < led_min || led >= led_max) continue;

            // The layer whose key is active here, looking through _______
            keypos_t pos     = {.col = col, .row = row};
            uint16_t keycode = keymap_key_to_keycode(layer_switch_get_layer(pos), pos);
            // RGB_MATRIX_MAXIMUM_BRIGHTNESS: the keyboard's cap (175 on the
            // Moonlander)
            rgb_matrix_set_color(led, keycode == FLASH_KEY ? RGB_MATRIX_MAXIMUM_BRIGHTNESS : 0, 0, 0);
        }
    }
    return false;
}
#endif

// When FLASH_KEY was pressed, or 0 if it isn't held
static uint16_t flash_pressed_at = 0;

void housekeeping_task_user(void) {
    if (flash_pressed_at && timer_elapsed(flash_pressed_at) >= FLASH_HOLD_MS) {
        flash_pressed_at = 0;
        reset_keyboard();
    }
}

// The Shift keys (Z and / on BASE, [ and - on SYMB) are Shift as soon as
// another key is pressed while they're held, so a capital typed quickly
// still works if Shift is released before the letter. The cost: fast rolls
// starting with these keys (e.g. "/a" in a path) can become capitals. Other
// mod-taps keep Permissive Hold.
bool get_hold_on_other_key_press(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case LSFT_T(KC_Z):
        case RSFT_T(KC_SLSH):
        case LSFT_T(KC_LBRC):
        case RSFT_T(KC_MINS):
            return true;
        default:
            return false;
    }
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case LALT_LABK:
        case LGUI_RABK:
            // Tapped: send < or > instead of , or . (held: Alt or Gui, as usual)
            if (record->tap.count && record->event.pressed) {
                tap_code16(keycode == LALT_LABK ? KC_LABK : KC_RABK);
                return false;
            }
            break;
        case FLASH_KEY:
            // | 1 so a press at timer 0 isn't mistaken for "not held"
            flash_pressed_at = record->event.pressed ? (timer_read() | 1) : 0;
            return false;
        case ESC_CAPS:
            // Held: toggle Caps Lock instead of momentarily switching to
            // layer 0. Tapped: falls through and sends Escape.
            if (!record->tap.count && record->event.pressed) {
                tap_code(KC_CAPS);
                return false;
            }
            break;
    }
    return true;
}
