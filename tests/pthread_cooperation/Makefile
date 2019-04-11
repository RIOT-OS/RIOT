include ../Makefile.tests_common

BOARD_BLACKLIST := arduino-duemilanove arduino-mega2560 arduino-nano \
                   arduino-uno jiminy-mega256rfr2 mega-xplained waspmote-pro \
                   hifive1
# arduino mega2560 uno duemilanove : unknown type name: clockid_t
# jiminy-mega256rfr2: unknown type name: clockid_t
# mega-xplained: unknown type name: clockid_t
# hifive1: not enough memory for thread stacks

BOARD_INSUFFICIENT_MEMORY := nucleo-f031k6

USEMODULE += posix_headers
USEMODULE += pthread

TEST_ON_CI_WHITELIST += all

include $(RIOTBASE)/Makefile.include
