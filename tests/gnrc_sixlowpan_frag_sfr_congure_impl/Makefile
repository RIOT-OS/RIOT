include ../Makefile.tests_common

USEMODULE += gnrc_netdev_default
USEMODULE += auto_init_gnrc_netif
USEMODULE += gnrc_ipv6_router_default
USEMODULE += gnrc_sixlowpan_frag_sfr
USEMODULE += gnrc_pktbuf_cmd
USEMODULE += gnrc_udp
USEMODULE += gnrc_rpl
USEMODULE += auto_init_gnrc_rpl
USEMODULE += gnrc_pktdump
USEMODULE += gnrc_icmpv6_echo
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps
USEMODULE += netstats_l2
USEMODULE += netstats_ipv6
USEMODULE += netstats_rpl

ifeq (,$(CONGURE_IMPL))
  USEMODULE += gnrc_sixlowpan_frag_sfr_congure_sfr
else
ifeq (congure_sfr,$(CONGURE_IMPL))
  USEMODULE += gnrc_sixlowpan_frag_sfr_congure_sfr
else
  $(error "Unknown CongURE implementation `$(CONGURE_IMPL)`")
endif
endif

include $(RIOTBASE)/Makefile.include

# Set a custom channel if needed
include $(RIOTMAKE)/default-radio-settings.inc.mk
