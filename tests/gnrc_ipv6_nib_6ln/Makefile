include ../Makefile.tests_common

USEMODULE += gnrc_ipv6
USEMODULE += gnrc_sixlowpan
USEMODULE += gnrc_sixlowpan_ctx
USEMODULE += gnrc_ipv6_nib_6ln
USEMODULE += gnrc_netif
USEMODULE += embunit
USEMODULE += netdev_ieee802154
USEMODULE += netdev_test

# lower log-level to save memory of LOG_WARNING() in gnrc_netif
CFLAGS += -DLOG_LEVEL=LOG_ERROR
CFLAGS += -DGNRC_NETTYPE_NDP=GNRC_NETTYPE_TEST
CFLAGS += -DTEST_SUITES

include $(RIOTBASE)/Makefile.include

# Set GNRC_PKTBUF_SIZE via CFLAGS if not being set via Kconfig.
ifndef CONFIG_GNRC_PKTBUF_SIZE
  CFLAGS += -DCONFIG_GNRC_PKTBUF_SIZE=512
endif
