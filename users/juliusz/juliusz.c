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
