MODULE := gnrc_netif_ethernet

MODULE_DEFS_NEEDED += \
  gnrc_ipv6 \
  gnrc_sixloenc \
  l2filter \
  netstats_l2 \
  od \
  #

# this module is expected to pass static analysis
MODULE_SUPPORTS_STATIC_ANALYSIS := 1

include $(RIOTBASE)/Makefile.base
