# name of your application
APPLICATION = gnrc_ipv6_nib
include ../Makefile.tests_common

BOARD_INSUFFICIENT_MEMORY := chronos nucleo32-f031 nucleo32-f042

USEMODULE += gnrc_ipv6
USEMODULE += gnrc_ipv6_nib
USEMODULE += embunit

CFLAGS += -DDEVELHELP
CFLAGS += -DGNRC_NETTYPE_NDP2=GNRC_NETTYPE_TEST
CFLAGS += -DGNRC_PKTBUF_SIZE=512
CFLAGS += -DTEST_SUITES

include $(RIOTBASE)/Makefile.include

test:
	tests/01-run.py
