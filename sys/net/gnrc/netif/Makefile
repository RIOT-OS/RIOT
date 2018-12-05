MODULE := gnrc_netif

SRC += _netif.c
SRC += gnrc_netif.c
SRC += gnrc_netif_raw.c

ifndef (,$(filter netdev_ieee802154,$(USEMODULE)))
  SRC += gnrc_netif_ieee802154.c
endif
ifndef (,$(filter netdev_ethernet,$(USEMODULE)))
  SRC += gnrc_netif_ethernet.c
endif

include $(RIOTBASE)/Makefile.base
