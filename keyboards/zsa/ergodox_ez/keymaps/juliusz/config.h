#pragma once

// Some keys chatter (register twice) with ZSA's default of 20 ms. With
// sym_eager_pk (see rules.mk) a longer debounce doesn't delay or drop key
// presses, it only stops a key from changing again within DEBOUNCE ms.
#undef DEBOUNCE
#define DEBOUNCE 40
