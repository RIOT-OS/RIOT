include ../Makefile.net_common

TEST_6LO ?= 1
TEST_FRAG_SFR ?= 1

ifeq (1,$(TEST_6LO))
  USEMODULE += gnrc_sixlowpan_default
  USEMODULE += netdev_ieee802154
  ifeq (1,$(TEST_FRAG_SFR))
    USEMODULE += gnrc_sixlowpan_frag_sfr
  endif
else
  USEMODULE += gnrc_ipv6_default
  USEMODULE += netdev_eth
endif

USEMODULE += gnrc_tx_sync
USEMODULE += netdev_test
USEMODULE += sock_udp
USEMODULE += xtimer
USEMODULE += auto_init_gnrc_netif
USEMODULE += iolist

# microbit qemu failing currently
TEST_ON_CI_BLACKLIST += microbit

include $(RIOTBASE)/Makefile.include

CFLAGS += -DCONFIG_ASSERT_VERBOSE=1
