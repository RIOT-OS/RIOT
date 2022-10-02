include ../Makefile.tests_common

APPLICATION_RUST_MODULE = rust_minimal
BASELIBS += $(APPLICATION_RUST_MODULE).module

FEATURES_REQUIRED += rust_target

# Testing on stable to ensure that no nightly features are needed for basic
# Rust usage.
CARGO_CHANNEL = stable

# Currently unknown, something related to the LED_PORT definition that doesn't
# pass C2Rust's transpilation
BOARD_BLACKLIST := ek-lm4f120xl

include $(RIOTBASE)/Makefile.include
