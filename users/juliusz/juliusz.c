// Custom key behavior shared by all keyboards: Esc/Caps Lock, Alt/< and
// Gui/>, DASH_TOGGLE, FLASH_KEY and its light (see layout.h for the
// keycodes).
//
// Owns keyboard_post_init_user and layer_state_set_user; keymaps add their
// own code with keyboard_post_init_keymap and layer_state_set_keymap.

#include QMK_KEYBOARD_H
#include "layout.h"

// DASH_TOGGLE: whether SYMB_DASH comes on together with SYMB. Saved in the
// user part of EEPROM, with a marker byte to tell it apart from whatever was
// there before (e.g. an older firmware's data).
#define DASH_EEPROM_MARKER 0xD5
static bool dash_on = false;

static void save_dash_setting(void) {
    eeconfig_update_user((DASH_EEPROM_MARKER << 8) | dash_on);
}

static void load_dash_setting(void) {
    uint32_t saved = eeconfig_read_user();
    dash_on        = ((saved >> 8) & 0xFF) == DASH_EEPROM_MARKER && (saved & 1);
}

__attribute__((weak)) layer_state_t layer_state_set_keymap(layer_state_t state) {
    return state;
}

layer_state_t layer_state_set_user(layer_state_t state) {
    // SYMB_DASH is on exactly when SYMB is and DASH_TOGGLE is on
    if (dash_on && layer_state_cmp(state, SYMB)) {
        state |= (layer_state_t)1 << SYMB_DASH;
    } else {
        state &= ~((layer_state_t)1 << SYMB_DASH);
    }
    return layer_state_set_keymap(state);
}

#ifdef RGB_MATRIX_ENABLE
// Key lights (Moonlander, Voyager): all off, except FLASH_KEY, which is red at
// full brightness while its layer is active.

static void init_key_lights(void) {
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

__attribute__((weak)) void keyboard_post_init_keymap(void) {}

void keyboard_post_init_user(void) {
    load_dash_setting();
#ifdef RGB_MATRIX_ENABLE
    init_key_lights();
#endif
    keyboard_post_init_keymap();
}

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
        case RSFT_T(KC_EQL):
            // SYMB_DASH's - (where - is on SYMB), but not the outer = on BASE
            return IS_LAYER_ON(SYMB_DASH);
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
        case DASH_TOGGLE:
            if (record->event.pressed) {
                dash_on = !dash_on;
                save_dash_setting();
                // Re-evaluate SYMB_DASH now: SYMB is held to press this key
                layer_state_set(layer_state);
            }
            return false;
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
