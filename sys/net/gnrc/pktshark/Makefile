MODULE = gnrc_pktshark

MODULE_DEFS_NEEDED += \
  auto_init_gnrc_pktshark \
  gnrc_nettype_icmpv6 \
  gnrc_nettype_tcp \
  gnrc_nettype_udp \
  gnrc_pktshark_4in6 \
  gnrc_pktshark_coap \
  gnrc_pktshark_icmpv6 \
  shell_cmd_gnrc_pktshark \
  #

# this module is expected to pass static analysis
MODULE_SUPPORTS_STATIC_ANALYSIS := 1

include $(RIOTBASE)/Makefile.base
