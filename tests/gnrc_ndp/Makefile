include ../Makefile.tests_common

BOARD_INSUFFICIENT_MEMORY := chronos nucleo-f031k6 nucleo-f042k6 nucleo-l031k6 \
                             telosb wsn430-v1_3b wsn430-v1_4

USEMODULE += gnrc_ipv6_nib_router
USEMODULE += gnrc_ndp
USEMODULE += gnrc_netif
USEMODULE += embunit
USEMODULE += netdev_test

CFLAGS += -DGNRC_NETTYPE_NDP=GNRC_NETTYPE_TEST
CFLAGS += -DGNRC_PKTBUF_SIZE=512
CFLAGS += -DTEST_SUITES

TEST_ON_CI_WHITELIST += all

include $(RIOTBASE)/Makefile.include

test:
	tests/01-run.py
