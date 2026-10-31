#include QMK_KEYBOARD_H
#include "layout.h"

#ifdef RGB_MATRIX_ENABLE
// Key lights (Moonlander, Voyager): keys that do something on the current
// layer are white at key_light_level, keys that do nothing are off.
// LIGHT_DN/LIGHT_UP step through KEY_LIGHT_LEVELS, LIGHT_LV types the level.

// Roughly even steps for the eye, which is much more sensitive to changes at
// the dim end. 175 is the Moonlander's maximum brightness.
static const uint8_t KEY_LIGHT_LEVELS[] = {0, 2, 4, 6, 8, 12, 16, 24, 32, 48, 64, 96, 128, 175};

// Saved in the user part of EEPROM: the level in the low byte, and a marker
// in the next one to tell it apart from whatever was there before (e.g. from
// Oryx firmware).
#    define KEY_LIGHT_EEPROM_MARKER 0x4C

static uint8_t key_light_level = KEY_LIGHT_DEFAULT_LEVEL;

static void set_key_light_level(uint8_t level) {
    key_light_level = level;
    eeconfig_update_user((KEY_LIGHT_EEPROM_MARKER << 8) | level);
}

static void type_key_light_level(void) {
    char    digits[4];
    uint8_t i     = sizeof(digits) - 1;
    uint8_t level = key_light_level;
    digits[i]     = '\0';
    do {
        digits[--i] = '0' + level % 10;
        level /= 10;
    } while (level);
    send_string("key light level: ");
    send_string(digits + i);
}

void keyboard_post_init_user(void) {
    uint32_t saved = eeconfig_read_user();
    if (((saved >> 8) & 0xFF) == KEY_LIGHT_EEPROM_MARKER) {
        key_light_level = saved & 0xFF;
    }

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
            uint8_t  level   = keycode == KC_NO ? 0 : key_light_level;
            rgb_matrix_set_color(led, level, level, level);
        }
    }
    return false;
}
#endif

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
#ifdef RGB_MATRIX_ENABLE
        case LIGHT_DN:
            if (record->event.pressed) {
                // The next level below the current one
                for (int8_t i = ARRAY_SIZE(KEY_LIGHT_LEVELS) - 1; i >= 0; i--) {
                    if (KEY_LIGHT_LEVELS[i] < key_light_level) {
                        set_key_light_level(KEY_LIGHT_LEVELS[i]);
                        break;
                    }
                }
            }
            return false;
        case LIGHT_UP:
            if (record->event.pressed) {
                // The next level above the current one
                for (uint8_t i = 0; i < ARRAY_SIZE(KEY_LIGHT_LEVELS); i++) {
                    if (KEY_LIGHT_LEVELS[i] > key_light_level) {
                        set_key_light_level(KEY_LIGHT_LEVELS[i]);
                        break;
                    }
                }
            }
            return false;
        case LIGHT_LV:
            if (record->event.pressed) {
                type_key_light_level();
            }
            return false;
#endif
    }
    return true;
}
