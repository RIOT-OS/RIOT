ifneq (,$(filter gnrc_dhcpv6,$(USEMODULE)))
  DIRS += application_layer/dhcpv6
endif
ifneq (,$(filter gnrc_icmpv6,$(USEMODULE)))
  DIRS += network_layer/icmpv6
endif
ifneq (,$(filter gnrc_icmpv6_echo,$(USEMODULE)))
  DIRS += network_layer/icmpv6/echo
endif
ifneq (,$(filter gnrc_icmpv6_error,$(USEMODULE)))
  DIRS += network_layer/icmpv6/error
endif
ifneq (,$(filter gnrc_ipv6,$(USEMODULE)))
  DIRS += network_layer/ipv6
endif
ifneq (,$(filter gnrc_ipv6_ext,$(USEMODULE)))
  DIRS += network_layer/ipv6/ext
endif
ifneq (,$(filter gnrc_ipv6_ext_frag,$(USEMODULE)))
  DIRS += network_layer/ipv6/ext/frag
endif
ifneq (,$(filter gnrc_ipv6_ext_opt,$(USEMODULE)))
  DIRS += network_layer/ipv6/ext/opt
endif
ifneq (,$(filter gnrc_ipv6_ext_rh,$(USEMODULE)))
  DIRS += network_layer/ipv6/ext/rh
endif
ifneq (,$(filter gnrc_ipv6_hdr,$(USEMODULE)))
  DIRS += network_layer/ipv6/hdr
endif
ifneq (,$(filter gnrc_ipv6_nib,$(USEMODULE)))
  DIRS += network_layer/ipv6/nib
endif
ifneq (,$(filter gnrc_ipv6_whitelist,$(USEMODULE)))
  DIRS += network_layer/ipv6/whitelist
endif
ifneq (,$(filter gnrc_ipv6_blacklist,$(USEMODULE)))
  DIRS += network_layer/ipv6/blacklist
endif
ifneq (,$(filter gnrc_ndp,$(USEMODULE)))
    DIRS += network_layer/ndp
endif
ifneq (,$(filter gnrc_netapi,$(USEMODULE)))
  DIRS += netapi
endif
ifneq (,$(filter gnrc_netif gnrc_netif_%,$(USEMODULE)))
    DIRS += netif
endif
ifneq (,$(filter gnrc_netreg,$(USEMODULE)))
  DIRS += netreg
endif
ifneq (,$(filter gnrc_mac,$(USEMODULE)))
  DIRS += link_layer/mac
endif
ifneq (,$(filter gnrc_pkt,$(USEMODULE)))
  DIRS += pkt
endif
ifneq (,$(filter gnrc_lwmac,$(USEMODULE)))
  DIRS += link_layer/lwmac
endif
ifneq (,$(filter gnrc_pktbuf_malloc,$(USEMODULE)))
    DIRS += pktbuf_malloc
endif
ifneq (,$(filter gnrc_lorawan,$(USEMODULE)))
    DIRS += link_layer/lorawan
endif
ifneq (,$(filter gnrc_gomach,$(USEMODULE)))
    DIRS += link_layer/gomach
endif
ifneq (,$(filter gnrc_pktbuf_static,$(USEMODULE)))
  DIRS += pktbuf_static
endif
ifneq (,$(filter gnrc_pktbuf,$(USEMODULE)))
  DIRS += pktbuf
endif
ifneq (,$(filter gnrc_priority_pktqueue,$(USEMODULE)))
  DIRS += priority_pktqueue
endif
ifneq (,$(filter gnrc_pktdump,$(USEMODULE)))
  DIRS += pktdump
endif
ifneq (,$(filter gnrc_rpl,$(USEMODULE)))
  DIRS += routing/rpl
endif
ifneq (,$(filter gnrc_rpl_srh,$(USEMODULE)))
  DIRS += routing/rpl/srh
endif
ifneq (,$(filter gnrc_rpl_p2p,$(USEMODULE)))
  DIRS += routing/rpl/p2p
endif
ifneq (,$(filter gnrc_sixlowpan,$(USEMODULE)))
  DIRS += network_layer/sixlowpan
endif
ifneq (,$(filter gnrc_sixlowpan_ctx,$(USEMODULE)))
  DIRS += network_layer/sixlowpan/ctx
endif
ifneq (,$(filter gnrc_sixlowpan_frag,$(USEMODULE)))
  DIRS += network_layer/sixlowpan/frag
endif
ifneq (,$(filter gnrc_sixlowpan_frag_fb,$(USEMODULE)))
  DIRS += network_layer/sixlowpan/frag/fb
endif
ifneq (,$(filter gnrc_sixlowpan_frag_minfwd,$(USEMODULE)))
  DIRS += network_layer/sixlowpan/frag/minfwd
endif
ifneq (,$(filter gnrc_sixlowpan_frag_rb,$(USEMODULE)))
  DIRS += network_layer/sixlowpan/frag/rb
endif
ifneq (,$(filter gnrc_sixlowpan_frag_sfr,$(USEMODULE)))
  DIRS += network_layer/sixlowpan/frag/sfr
endif
ifneq (,$(filter gnrc_sixlowpan_frag_stats,$(USEMODULE)))
  DIRS += network_layer/sixlowpan/frag/stats
endif
ifneq (,$(filter gnrc_sixlowpan_frag_vrb,$(USEMODULE)))
  DIRS += network_layer/sixlowpan/frag/vrb
endif
ifneq (,$(filter gnrc_sixlowpan_iphc,$(USEMODULE)))
  DIRS += network_layer/sixlowpan/iphc
endif
ifneq (,$(filter gnrc_sixlowpan_nd,$(USEMODULE)))
  DIRS += network_layer/sixlowpan/nd
endif
ifneq (,$(filter gnrc_sock,$(USEMODULE)))
  DIRS += sock
endif
ifneq (,$(filter gnrc_sock_ip,$(USEMODULE)))
  DIRS += sock/ip
endif
ifneq (,$(filter gnrc_sock_udp,$(USEMODULE)))
  DIRS += sock/udp
endif
ifneq (,$(filter gnrc_udp,$(USEMODULE)))
  DIRS += transport_layer/udp
endif
ifneq (,$(filter gnrc_tcp,$(USEMODULE)))
  DIRS += transport_layer/tcp
endif
ifneq (,$(filter gnrc_tx_sync,$(USEMODULE)))
  DIRS += tx_sync
endif

include $(RIOTBASE)/Makefile.base
