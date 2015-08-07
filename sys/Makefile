ifneq (,$(filter pnet,$(USEMODULE)))
    DIRS += posix/pnet
endif
ifneq (,$(filter pthread,$(USEMODULE)))
    DIRS += posix/pthread
endif
ifneq (,$(filter shell_commands,$(USEMODULE)))
    DIRS += shell/commands
endif
ifneq (,$(filter net_help,$(USEMODULE)))
    DIRS += net/crosslayer/net_help
endif
ifneq (,$(filter routing,$(USEMODULE)))
    DIRS += net/routing
endif
ifneq (,$(filter oneway_malloc,$(USEMODULE)))
    DIRS += oneway-malloc
endif
ifneq (,$(filter ng_icmpv6,$(USEMODULE)))
    DIRS += net/network_layer/ng_icmpv6
endif
ifneq (,$(filter ng_icmpv6_echo,$(USEMODULE)))
    DIRS += net/network_layer/ng_icmpv6/echo
endif
ifneq (,$(filter ng_ipv6,$(USEMODULE)))
    DIRS += net/network_layer/ng_ipv6
endif
ifneq (,$(filter ng_ipv6_addr,$(USEMODULE)))
    DIRS += net/network_layer/ng_ipv6/addr
endif
ifneq (,$(filter ng_ipv6_ext,$(USEMODULE)))
    DIRS += net/network_layer/ng_ipv6/ext
endif
ifneq (,$(filter ng_ipv6_ext_rh,$(USEMODULE)))
    DIRS += net/network_layer/ng_ipv6/ext/rh
endif
ifneq (,$(filter ng_ipv6_hdr,$(USEMODULE)))
    DIRS += net/network_layer/ng_ipv6/hdr
endif
ifneq (,$(filter ng_ipv6_nc,$(USEMODULE)))
    DIRS += net/network_layer/ng_ipv6/nc
endif
ifneq (,$(filter ng_ipv6_netif,$(USEMODULE)))
    DIRS += net/network_layer/ng_ipv6/netif
endif
ifneq (,$(filter inet_csum,$(USEMODULE)))
    DIRS += net/crosslayer/inet_csum
endif
ifneq (,$(filter ng_ndp,$(USEMODULE)))
    DIRS += net/network_layer/ng_ndp
endif
ifneq (,$(filter ng_netapi,$(USEMODULE)))
    DIRS += net/crosslayer/ng_netapi
endif
ifneq (,$(filter ng_netif,$(USEMODULE)))
    DIRS += net/crosslayer/ng_netif
endif
ifneq (,$(filter ng_netif_hdr,$(USEMODULE)))
    DIRS += net/crosslayer/ng_netif/hdr
endif
ifneq (,$(filter ng_netreg,$(USEMODULE)))
    DIRS += net/crosslayer/ng_netreg
endif
ifneq (,$(filter ng_nettest,$(USEMODULE)))
    DIRS += net/crosslayer/ng_nettest
endif
ifneq (,$(filter ng_nomac,$(USEMODULE)))
    DIRS += net/link_layer/ng_nomac
endif
ifneq (,$(filter cipher_modes,$(USEMODULE)))
    DIRS += crypto/modes
endif
ifneq (,$(filter ng_pktbuf_static,$(USEMODULE)))
    DIRS += net/crosslayer/ng_pktbuf_static
endif
ifneq (,$(filter ng_zep,$(USEMODULE)))
    DIRS += net/application_layer/ng_zep
endif
ifneq (,$(filter ng_rpl_srh,$(USEMODULE)))
    DIRS += net/routing/ng_rpl/srh
endif
ifneq (,$(filter ng_sixlowpan,$(USEMODULE)))
    DIRS += net/network_layer/ng_sixlowpan
endif
ifneq (,$(filter ng_sixlowpan_ctx,$(USEMODULE)))
    DIRS += net/network_layer/ng_sixlowpan/ctx
endif
ifneq (,$(filter ng_sixlowpan_frag,$(USEMODULE)))
    DIRS += net/network_layer/ng_sixlowpan/frag
endif
ifneq (,$(filter ng_sixlowpan_iphc,$(USEMODULE)))
    DIRS += net/network_layer/ng_sixlowpan/iphc
endif
ifneq (,$(filter ng_sixlowpan_netif,$(USEMODULE)))
    DIRS += net/network_layer/ng_sixlowpan/netif
endif
ifneq (,$(filter ng_slip,$(USEMODULE)))
    DIRS += net/link_layer/ng_slip
endif
ifneq (,$(filter nhdp,$(USEMODULE)))
    DIRS += net/routing/nhdp
endif
ifneq (,$(filter ng_pktdump,$(USEMODULE)))
    DIRS += net/crosslayer/ng_pktdump
endif
ifneq (,$(filter fib,$(USEMODULE)))
    DIRS += net/network_layer/fib
endif
ifneq (,$(filter ng_udp,$(USEMODULE)))
    DIRS += net/transport_layer/ng_udp
endif
ifneq (,$(filter hwtimer_compat,$(USEMODULE)))
    DIRS += compat/hwtimer
endif
ifneq (,$(filter log_%,$(USEMODULE)))
    DIRS += log
endif
ifneq (,$(filter cpp11-compat,$(USEMODULE)))
    DIRS += cpp11-compat
endif
ifneq (,$(filter ng_netdev_eth,$(USEMODULE)))
    DIRS += net/link_layer/ng_netdev_eth
endif

DIRS += $(dir $(wildcard $(addsuffix /Makefile, ${USEMODULE})))

include $(RIOTBASE)/Makefile.base
