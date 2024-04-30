include ../Makefile.tests_common

USEMODULE += shell
USEMODULE += shell_democommands
USEMODULE += ztimer_msec

FEATURES_REQUIRED += rust_target

# Currently unknown, something related to the LED_PORT definition that doesn't
# pass C2Rust's transpilation
BOARD_BLACKLIST := ek-lm4f120xl

include $(RIOTBASE)/Makefile.include
