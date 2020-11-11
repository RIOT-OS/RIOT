include ../Makefile.tests_common

USEMODULE += gnrc_ipv6
USEMODULE += gnrc_ipv6_nib
USEMODULE += gnrc_netif
USEMODULE += embunit
USEMODULE += netdev_eth
USEMODULE += netdev_test

CFLAGS += -DGNRC_NETTYPE_NDP=GNRC_NETTYPE_TEST
CFLAGS += -DTEST_SUITES

include $(RIOTBASE)/Makefile.include

# Set GNRC_PKTBUF_SIZE via CFLAGS if not being set via Kconfig.
ifndef CONFIG_GNRC_PKTBUF_SIZE
  CFLAGS += -DCONFIG_GNRC_PKTBUF_SIZE=512
endif
