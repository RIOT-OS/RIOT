# name of your application
APPLICATION = gnrc_ipv6_nib
include ../Makefile.tests_common

BOARD_INSUFFICIENT_MEMORY :=

USEMODULE += embunit
USEMODULE += gnrc_netif2
USEMODULE += gnrc_pktdump
USEMODULE += gnrc_sixlowpan
USEMODULE += gnrc_sixlowpan_iphc
USEMODULE += gnrc_ipv6
USEMODULE += netdev_eth
USEMODULE += netdev_test
USEMODULE += od

CFLAGS += -DDEVELHELP
CFLAGS += -DGNRC_NETIF_NUMOF=4
CFLAGS += -DGNRC_NETIF2_ADDRS_NUMOF=16
CFLAGS += -DGNRC_NETIF2_GROUPS_NUMOF=8
CFLAGS += -DLOG_LEVEL=LOG_NONE
CFLAGS += -DTEST_SUITES

include $(RIOTBASE)/Makefile.include

test:
# `testrunner` calls `make term` recursively, results in duplicated `TERMFLAGS`.
# So clears `TERMFLAGS` before run.
	TERMFLAGS= tests/01-run.py
