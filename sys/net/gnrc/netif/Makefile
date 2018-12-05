MODULE := gnrc_netif

ifneq (,$(filter gnrc_netif_ethernet,$(USEMODULE)))
  DIRS += ethernet
endif
ifneq (,$(filter gnrc_netif_ieee802154,$(USEMODULE)))
  DIRS += ieee802154
endif

include $(RIOTBASE)/Makefile.base
