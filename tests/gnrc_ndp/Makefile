include ../Makefile.tests_common

USEMODULE += gnrc_ipv6_nib_router
USEMODULE += gnrc_ndp
USEMODULE += gnrc_netif
USEMODULE += gnrc_nettype_ipv6
USEMODULE += embunit
USEMODULE += netdev_ieee802154
USEMODULE += netdev_test

CFLAGS += -DGNRC_NETTYPE_NDP=GNRC_NETTYPE_TEST
CFLAGS += -DTEST_SUITES

INCLUDES += -I$(RIOTBASE)/sys/net/gnrc/pktbuf_static/include

include $(RIOTBASE)/Makefile.include

# Set GNRC_PKTBUF_SIZE via CFLAGS if not being set via Kconfig.
ifndef CONFIG_GNRC_PKTBUF_SIZE
  CFLAGS += -DCONFIG_GNRC_PKTBUF_SIZE=512
endif
