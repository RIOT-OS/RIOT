DEVELHELP = 0

include ../Makefile.net_common

USEMODULE += embunit
USEMODULE += l2util

# enable all code paths in l2util without pulling in the actual
# drivers
CFLAGS += -DMODULE_CC110X \
          -DMODULE_ESP_NOW \
          -DMODULE_NETDEV_ETH \
          -DMODULE_NETDEV_IEEE802154 \
          -DMODULE_NIMBLE_NETIF \
          -DMODULE_NRFMIN \
          -DMODULE_XBEE

include $(RIOTBASE)/Makefile.include
