# name of your application
APPLICATION = gnrc_ndp2
include ../Makefile.tests_common

BOARD_INSUFFICIENT_MEMORY := nucleo32-f031 nucleo32-f042

USEMODULE += gnrc_ipv6_nib
USEMODULE += gnrc_ndp2
USEMODULE += gnrc_netif2
USEMODULE += embunit
USEMODULE += netdev_test

CFLAGS += -DGNRC_NETTYPE_NDP2=GNRC_NETTYPE_TEST
CFLAGS += -DGNRC_PKTBUF_SIZE=512
CFLAGS += -DTEST_SUITES

include $(RIOTBASE)/Makefile.include

test:
	tests/01-run.py
