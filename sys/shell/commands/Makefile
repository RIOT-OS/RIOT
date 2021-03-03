MODULE = shell_commands

SRC = shell_commands.c cmd_sys.c

ifneq (,$(filter app_metadata,$(USEMODULE)))
  SRC += cmd_app_metadata.c
endif
ifneq (,$(filter dfplayer,$(USEMODULE)))
  SRC += cmd_dfplayer.c
endif
ifneq (,$(filter mci,$(USEMODULE)))
  SRC += cmd_disk.c
endif
ifneq (,$(filter periph_pm,$(USEMODULE)))
  SRC += cmd_pm.c
endif
ifneq (,$(filter ps,$(USEMODULE)))
  SRC += cmd_ps.c
endif
ifneq (,$(filter heap_cmd,$(USEMODULE)))
  SRC += cmd_heap.c
endif
ifneq (,$(filter sht1x,$(USEMODULE)))
  SRC += cmd_sht1x.c
endif
ifneq (,$(filter lpc2387,$(USEMODULE)))
  SRC += cmd_heap.c
endif
ifneq (,$(filter random,$(USEMODULE)))
  SRC += cmd_random.c
endif
ifneq (,$(filter at30tse75x,$(USEMODULE)))
    SRC += cmd_at30tse75x.c
endif
ifneq (,$(filter gnrc_netif,$(USEMODULE)))
  SRC += cmd_gnrc_netif.c
endif
ifneq (,$(filter netstats_neighbor,$(USEMODULE)))
  SRC += cmd_netstats_nb.c
endif
ifneq (,$(filter fib,$(USEMODULE)))
  SRC += cmd_fib.c
endif
ifneq (,$(filter gnrc_ipv6_ext_frag_stats,$(USEMODULE)))
  SRC += cmd_gnrc_ipv6_frag_stats.c
endif
ifneq (,$(filter gnrc_ipv6_nib,$(USEMODULE)))
  SRC += cmd_gnrc_ipv6_nib.c
endif
ifneq (,$(filter gnrc_ipv6_whitelist,$(USEMODULE)))
  SRC += cmd_whitelist.c
endif
ifneq (,$(filter gnrc_ipv6_blacklist,$(USEMODULE)))
  SRC += cmd_blacklist.c
endif
ifneq (,$(filter gnrc_icmpv6_echo,$(USEMODULE)))
ifneq (,$(filter xtimer,$(USEMODULE)))
  SRC += cmd_gnrc_icmpv6_echo.c
endif
endif
ifneq (,$(filter gnrc_pktbuf_cmd,$(USEMODULE)))
    SRC += cmd_gnrc_pktbuf.c
endif
ifneq (,$(filter gnrc_rpl,$(USEMODULE)))
    SRC += cmd_gnrc_rpl.c
endif
ifneq (,$(filter gnrc_sixlowpan_ctx,$(USEMODULE)))
    SRC += cmd_gnrc_6ctx.c
endif
ifneq (,$(filter gnrc_sixlowpan_frag_stats,$(USEMODULE)))
  SRC += cmd_gnrc_6lo_frag_stats.c
endif
ifneq (,$(filter saul_reg,$(USEMODULE)))
  SRC += cmd_saul_reg.c
endif
ifneq (,$(filter ccn-lite-utils,$(USEMODULE)))
  SRC += cmd_ccnl.c
endif
ifneq (,$(filter sntp,$(USEMODULE)))
  SRC += cmd_sntp.c
endif
ifneq (,$(filter vfs,$(USEMODULE)))
  SRC += cmd_vfs.c
endif
ifneq (,$(filter conn_can,$(USEMODULE)))
  SRC += cmd_can.c
endif
ifneq (,$(filter cord_ep,$(USEMODULE)))
  SRC += cmd_cord_ep.c
endif

ifneq (,$(filter openwsn,$(USEPKG)))
  SRC += cmd_openwsn.c
endif

ifneq (,$(filter lwip_netif,$(USEMODULE)))
  SRC += sc_lwip_netif.c
endif

ifneq (,$(filter periph_rtc,$(USEMODULE)))
  SRC += cmd_rtc.c
endif

ifneq (,$(filter rtt_cmd,$(USEMODULE)))
  SRC += cmd_rtt.c
endif

ifneq (,$(filter i2c_scan,$(USEMODULE)))
  SRC += cmd_i2c_scan.c
endif

ifneq (,$(filter semtech-loramac,$(USEPKG)))
  SRC += cmd_loramac.c
endif

ifneq (,$(filter nimble_netif,$(USEMODULE)))
  SRC += cmd_nimble_netif.c
endif

ifneq (,$(filter nimble_statconn,$(USEMODULE)))
  SRC += cmd_nimble_statconn.c
endif

ifneq (,$(filter suit_transport_coap,$(USEMODULE)))
  SRC += cmd_suit.c
endif

ifneq (,$(filter cryptoauthlib,$(USEPKG)))
  SRC += cmd_cryptoauthlib.c
endif

include $(RIOTBASE)/Makefile.base
