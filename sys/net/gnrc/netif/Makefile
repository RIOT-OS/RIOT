MODULE := gnrc_netif

ifneq (,$(filter gnrc_netif_ethernet,$(USEMODULE)))
  DIRS += ethernet
endif
ifneq (,$(filter gnrc_netif_ieee802154,$(USEMODULE)))
  DIRS += ieee802154
endif
ifneq (,$(filter gnrc_netif_ieee802154_mac,$(USEMODULE)))
  DIRS += ieee802154_mac
endif
ifneq (,$(filter gnrc_netif_init_devs,$(USEMODULE)))
  DIRS += init_devs
endif
ifneq (,$(filter gnrc_netif_pktq,$(USEMODULE)))
  DIRS += pktq
endif
ifneq (,$(filter gnrc_netif_hdr,$(USEMODULE)))
  DIRS += hdr
endif
ifneq (,$(filter gnrc_netif_lorawan,$(USEMODULE)))
  DIRS += lorawan
endif

# this module is expected to pass static analysis
MODULE_SUPPORTS_STATIC_ANALYSIS := 1

include $(RIOTBASE)/Makefile.base
