// The layout, shared by all keyboards.
//
// Layers are written in the shape of the ErgoDox EZ (the order of
// LAYOUT_ergodox_pretty), the biggest of the supported keyboards. wrappers.h
// maps them onto the Moonlander and Voyager, skipping the keys they don't
// have. See ../../README.md for which keys end up where.
//
// To add a layer: give it a number, define LAYER_<NAME> below and add it to
// KEYMAPS. (Layer numbers are #defines rather than an enum so that the image
// generator sees numbers in LT(), MO() etc.)

#pragma once

#define BASE 0
#define SYMB 1

// Escape on tap, toggles Caps Lock when held past TAPPING_TERM (see
// juliusz.c). LT(0, ...) is just a way to get a tap-hold key with a custom
// hold action.
#define ESC_CAPS LT(0, KC_ESC)

// clang-format off

#define LAYER_BASE \
    KC_GRV,           KC_1,             KC_2,             KC_3,             KC_4,             KC_5,             XXXXXXX,              XXXXXXX,          KC_6,             KC_7,             KC_8,             KC_9,             KC_0,             KC_MINS,        \
    KC_TAB,           KC_Q,             KC_W,             KC_E,             KC_R,             KC_T,             XXXXXXX,              XXXXXXX,          KC_Y,             KC_U,             KC_I,             KC_O,             KC_P,             KC_BSLS,        \
    ESC_CAPS,         KC_A,             KC_S,             KC_D,             KC_F,             KC_G,                                                     KC_H,             KC_J,             KC_K,             KC_L,             KC_SCLN,          KC_QUOT,        \
    LSFT_T(KC_ESC),   LSFT_T(KC_Z),     LCTL_T(KC_X),     LALT_T(KC_C),     LGUI_T(KC_V),     KC_B,             XXXXXXX,              XXXXXXX,          KC_N,             RGUI_T(KC_M),     RALT_T(KC_COMM),  RCTL_T(KC_DOT),   RSFT_T(KC_SLSH),  RSFT_T(KC_EQL), \
    XXXXXXX,          XXXXXXX,          XXXXXXX,          XXXXXXX,          LGUI_T(KC_ESC),                                                                               KC_RALT,          XXXXXXX,          XXXXXXX,          XXXXXXX,          XXXXXXX,        \
                                                                                              XXXXXXX,          XXXXXXX,              XXXXXXX,          XXXXXXX,                                                                                                  \
                                                                                                                XXXXXXX,              XXXXXXX,                                                                                                                    \
                                                                            LT(SYMB, KC_SPC), KC_BSPC,          XXXXXXX,              XXXXXXX,          KC_ENT,           LT(SYMB, KC_SPC)

#define LAYER_SYMB \
    _______,          KC_F1,            KC_F2,            KC_F3,            KC_F4,            KC_F5,            _______,              _______,          KC_F6,            KC_F7,            KC_F8,            KC_F9,            KC_F10,           KC_F11,  \
    _______,          KC_BRID,          KC_BRIU,          _______,          _______,          _______,          _______,              _______,          KC_MPRV,          KC_MNXT,          KC_MUTE,          KC_VOLD,          KC_VOLU,          KC_F12,  \
    _______,          KC_LCBR,          KC_RCBR,          KC_LPRN,          KC_RPRN,          KC_UNDS,                                                  KC_LEFT,          KC_DOWN,          KC_UP,            KC_RGHT,          KC_UNDS,          _______, \
    _______,          KC_LBRC,          KC_RBRC,          KC_LABK,          KC_RABK,          KC_MINS,          _______,              _______,          KC_HOME,          KC_PGDN,          KC_PGUP,          KC_END,           KC_MINS,          _______, \
    _______,          _______,          _______,          _______,          _______,                                                                                      _______,          _______,          _______,          _______,          _______, \
                                                                                              _______,          _______,              _______,          _______,                                                                                           \
                                                                                                                _______,              _______,                                                                                                             \
                                                                            _______,          KC_DEL,           _______,              _______,          _______,          _______

// clang-format on

#define KEYMAPS(layout)            \
    [BASE] = layout(LAYER_BASE), \
    [SYMB] = layout(LAYER_SYMB)
