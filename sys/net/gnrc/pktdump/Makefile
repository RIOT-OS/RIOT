MODULE = gnrc_pktdump

MODULE_DEFS_NEEDED += \
  gnrc_netif_hdr \
  gnrc_nettype_ccn \
  gnrc_nettype_icmpv6 \
  gnrc_nettype_ipv6 \
  gnrc_nettype_ipv6_ext \
  gnrc_nettype_ndn \
  gnrc_nettype_sixlowpan \
  gnrc_nettype_tcp \
  gnrc_nettype_udp \
  icmpv6 \
  ipv6_hdr \
  sixlowpan \
  tcp \
  udp \
  #

# this module is expected to pass static analysis
MODULE_SUPPORTS_STATIC_ANALYSIS := 1

include $(RIOTBASE)/Makefile.base
