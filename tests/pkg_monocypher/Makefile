include ../Makefile.tests_common

# No 8 bit and 16 bit support
BOARD_BLACKLIST := arduino-duemilanove arduino-mega2560 arduino-uno chronos \
        jiminy-mega256rfr2 mega-xplained msb-430 msb-430h telosb \
        waspmote-pro wsn430-v1_3b wsn430-v1_4 z1

BOARD_INSUFFICIENT_MEMORY := nucleo-f031k6 nucleo-f042k6

# required for Monocypher (as described in the package documentation)
CFLAGS += "-DTHREAD_STACKSIZE_MAIN=(3072 + THREAD_STACKSIZE_DEFAULT + THREAD_EXTRA_STACKSIZE_PRINTF)"

USEMODULE += embunit
USEMODULE += random
USEPKG += monocypher

TEST_ON_CI_WHITELIST += all
include $(RIOTBASE)/Makefile.include

test:
	tests/01-run.py
