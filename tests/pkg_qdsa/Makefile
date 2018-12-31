include ../Makefile.tests_common

TEST_ON_CI_WHITELIST += all

# qDSA is not 16 bit compatible
BOARD_BLACKLIST := chronos msb-430 msb-430h telosb wsn430-v1_3b wsn430-v1_4 z1

BOARD_INSUFFICIENT_MEMORY := arduino-duemilanove arduino-uno nucleo-f031k6

CFLAGS += -DTHREAD_STACKSIZE_MAIN=\(4*THREAD_STACKSIZE_DEFAULT\)

USEPKG += qDSA
USEMODULE += random
USEMODULE += embunit

include $(RIOTBASE)/Makefile.include
