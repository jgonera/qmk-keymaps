#include QMK_KEYBOARD_H
#include "layout.h"

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
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

#ifdef RGB_MATRIX_ENABLE
// Key lights (Moonlander, Voyager): keys that do something on the current
// layer are white at KEY_LIGHT_LEVEL, keys that do nothing are off.

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
            uint8_t  level   = keycode == KC_NO ? 0 : KEY_LIGHT_LEVEL;
            rgb_matrix_set_color(led, level, level, level);
        }
    }
    return false;
}
#endif
