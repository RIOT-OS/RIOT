include ../Makefile.tests_common

USEMODULE += shell
USEMODULE += shell_democommands

FEATURES_REQUIRED += rust_target

ifeq (native, $(BOARD))
  RIOT_TERMINAL ?= native
endif

# Testing on stable to ensure that no nightly features are needed when Rust is
# pulled in through modules.
CARGO_CHANNEL = stable

# Currently unknown, something related to the LED_PORT definition that doesn't
# pass C2Rust's transpilation
BOARD_BLACKLIST := ek-lm4f120xl

include $(RIOTBASE)/Makefile.include
