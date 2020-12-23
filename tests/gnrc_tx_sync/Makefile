include ../Makefile.tests_common

TEST_6LO ?= 1

ifeq (1,$(TEST_6LO))
  USEMODULE += gnrc_sixlowpan_default
  USEMODULE += netdev_ieee802154
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

include $(RIOTBASE)/Makefile.include

CFLAGS += -DDEBUG_ASSERT_VERBOSE=1
