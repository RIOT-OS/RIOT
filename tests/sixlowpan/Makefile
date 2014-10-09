export APPLICATION = sixlowpan

include ../Makefile.tests_common

BOARD_INSUFFICIENT_RAM := chronos msb-430 msb-430h stm32f0discovery redbee-econotag \
                          telosb wsn430-v1_3b wsn430-v1_4 z1

BOARD_BLACKLIST := arduino-mega2560 # errnos all have the same value
                                    # => switch-case in framework.c does not compile

USEMODULE += netapi
USEMODULE += sixlowpan

ifeq (,$(WITHOUT_SIXLOWPAN_HC))
    USEMODULE += sixlowpan_hc
endif

ifeq (TEXT,$(OUTPUT))
    CFLAGS += -DTEXT
endif

DISABLE_MODULE += auto_init

CFLAGS += -DRANDOM_BYTE=$(shell od -vAn -N1 -tu1 < /dev/urandom | xargs echo)
CFLAGS += -DRANDOM_SHORT=$(shell od -vAn -N2 -tu2 < /dev/urandom | xargs echo)
CFLAGS += -DTEST_SUITES

include $(RIOTBASE)/Makefile.include

test:
	./tests/01-tests.py
