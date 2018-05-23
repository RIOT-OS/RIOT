include ../Makefile.tests_common

BOARD_BLACKLIST := arduino-mega2560 waspmote-pro arduino-uno arduino-duemilanove \
                   jiminy-mega256rfr2 mega-xplained
# arduino mega2560 uno duemilanove : unknown type name: clockid_t
# jiminy-mega256rfr2: unknown type name: clockid_t
# mega-xplained: unknown type name: clockid_t

# exclude boards with insufficient memory
BOARD_INSUFFICIENT_MEMORY := nucleo-f031k6

# Modules to include.
USEMODULE += pthread
USEMODULE += random
USEMODULE += xtimer

TEST_ON_CI_WHITELIST += all

include $(RIOTBASE)/Makefile.include

test:
	tests/01-run.py
