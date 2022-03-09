include ../Makefile.tests_common

USEMODULE += embunit
USEMODULE += gnrc_ipv6_nib_6ln
USEMODULE += gnrc_sixlowpan_iphc
USEMODULE += gnrc_sixlowpan_frag
USEMODULE += gnrc_sixlowpan_frag_vrb
USEMODULE += netdev_ieee802154
USEMODULE += netdev_test
USEMODULE += od

# we don't need all this packet buffer space so reduce it a little
CFLAGS += -DTEST_SUITES

include $(RIOTBASE)/Makefile.include

# Set GNRC_PKTBUF_SIZE via CFLAGS if not being set via Kconfig.
ifndef CONFIG_GNRC_PKTBUF_SIZE
  CFLAGS += -DCONFIG_GNRC_PKTBUF_SIZE=2048
endif
