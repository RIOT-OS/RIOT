MODULE := gnrc_netif

MODULE_DEFS_NEEDED += \
  cc110x \
  esp_now \
  gnrc_ipv6 \
  gnrc_ipv6_nib \
  gnrc_netif_6lo \
  gnrc_netif_bus \
  gnrc_netif_ipv6 \
  gnrc_netif_pktq \
  gnrc_sixloenc \
  gnrc_sixlowpan_frag_sfr \
  gnrc_sixlowpan_iphc \
  gnrc_sixlowpan_nd \
  gnrc_tx_sync \
  netdev_eth \
  netdev_ieee802154 \
  netdev_legacy_api \
  netdev_new_api \
  netdev_register \
  netopt \
  netstats_ipv6 \
  netstats_l2 \
  netstats_neighbor \
  nimble_netif \
  nrf24l01p_ng \
  nrfmin \
  slipdev_l2addr \
  xbee \
  ztimer \
  #

ifneq (,$(filter gnrc_netif_ethernet,$(USEMODULE)))
  DIRS += ethernet
endif
ifneq (,$(filter gnrc_netif_ieee802154,$(USEMODULE)))
  DIRS += ieee802154
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
