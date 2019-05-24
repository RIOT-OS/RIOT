include ../Makefile.tests_common

BOARD_BLACKLIST := arduino-duemilanove arduino-leonardo arduino-mega2560 \
                   arduino-nano arduino-uno i-nucleo-lrwan1 jiminy-mega256rfr2 \
                   mega-xplained waspmote-pro
# AVR platform: unknown type name: clockid_t

# exclude boards with insufficient memory
BOARD_INSUFFICIENT_MEMORY := nucleo-f031k6

# Modules to include.
USEMODULE += pthread
USEMODULE += random
USEMODULE += xtimer

TEST_ON_CI_WHITELIST += all

include $(RIOTBASE)/Makefile.include
