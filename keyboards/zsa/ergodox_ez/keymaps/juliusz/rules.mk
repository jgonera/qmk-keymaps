# Register each key change immediately, then ignore that key's bouncing for
# DEBOUNCE ms (ZSA's default, 20). QMK's default, sym_defer_g, only registers
# changes once the whole keyboard has been still for DEBOUNCE ms, which
# swallowed quick key presses when typing fast.
DEBOUNCE_TYPE = sym_eager_pk
