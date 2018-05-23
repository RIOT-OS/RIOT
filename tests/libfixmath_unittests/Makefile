include ../Makefile.tests_common

BOARD_BLACKLIST := arduino-mega2560 waspmote-pro arduino-uno arduino-duemilanove \
                   jiminy-mega256rfr2 mega-xplained
# arduino-mega2560: builds locally but breaks travis (possibly because of
# differences in the toolchain)
# The MSP boards don't feature round(), exp(), and log(), which are used in the unittests
BOARD_BLACKLIST += chronos msb-430 msb-430h telosb wsn430-v1_3b wsn430-v1_4 z1

BOARD_INSUFFICIENT_MEMORY := nucleo-f031k6 nucleo-f042k6 nucleo-l031k6

USEMODULE += libfixmath-unittests

ifneq (,$(filter native,$(BOARD)))
  export LINKFLAGS += -lm
endif

USEMODULE += printf_float

TEST_ON_CI_WHITELIST += native

include $(RIOTBASE)/Makefile.include

test:
	tests/01-run.py
