include ../Makefile.tests_common

BOARD_BLACKLIST := arduino-duemilanove arduino-leonardo arduino-mega2560 \
                   arduino-nano arduino-uno hifive1 i-nucleo-lrwan1 \
                   jiminy-mega256rfr2 mega-xplained waspmote-pro

# AVR platform: unknown type name: clockid_t
# hifive1: not enough memory for thread stacks

BOARD_INSUFFICIENT_MEMORY := nucleo-f031k6

USEMODULE += posix_headers
USEMODULE += pthread

TEST_ON_CI_WHITELIST += all

include $(RIOTBASE)/Makefile.include
