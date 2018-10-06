include ../Makefile.tests_common

BOARD_BLACKLIST := arduino-duemilanove \
                   arduino-mega2560 \
                   arduino-uno \
                   jiminy-mega256rfr2 \
                   mega-xplained \
                   waspmote-pro
# arduino mega2560 uno duemilanove : unknown type name: clockid_t
# jiminy-mega256rfr2: unknown type name: clockid_t
# mega-xplained: unknown type name: clockid_t

BOARD_INSUFFICIENT_MEMORY := nucleo-f031k6

USEMODULE += posix
USEMODULE += pthread

TEST_ON_CI_WHITELIST += all

include $(RIOTBASE)/Makefile.include
