# These tests hit an assert when used with gnrc_netif, but they exactly for
# testing this netdev simulating framework, so I it's better if the assert is
# ignored.
CFLAGS += -DNDEBUG
include ../Makefile.tests_common

DISABLE_MODULE += auto_init_gnrc_%

USEMODULE += gnrc
USEMODULE += gnrc_neterr
USEMODULE += gnrc_netif
USEMODULE += netdev_eth
USEMODULE += netdev_test
USEMODULE += od

include $(RIOTBASE)/Makefile.include

# Set GNRC_PKTBUF_SIZE via CFLAGS if not being set via Kconfig.
ifndef CONFIG_GNRC_PKTBUF_SIZE
  CFLAGS += -DCONFIG_GNRC_PKTBUF_SIZE=200
endif
