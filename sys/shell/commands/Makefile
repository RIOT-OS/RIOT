MODULE = shell_commands

SRC = shell_commands.c sc_sys.c

ifneq (,$(filter mci,$(USEMODULE)))
  SRC += sc_disk.c
endif
ifneq (,$(filter ps,$(USEMODULE)))
  SRC += sc_ps.c
endif
ifneq (,$(filter sht1x,$(USEMODULE)))
  SRC += sc_sht1x.c
endif
ifneq (,$(filter lpc2387,$(USEMODULE)))
  SRC += sc_heap.c
endif
ifneq (,$(filter random,$(USEMODULE)))
  SRC += sc_random.c
endif
ifneq (,$(filter at30tse75x,$(USEMODULE)))
    SRC += sc_at30tse75x.c
endif
ifneq (,$(filter gnrc_netif,$(USEMODULE)))
  SRC += sc_gnrc_netif.c
endif
ifneq (,$(filter fib,$(USEMODULE)))
  SRC += sc_fib.c
endif
ifneq (,$(filter gnrc_ipv6_nib,$(USEMODULE)))
  SRC += sc_gnrc_ipv6_nib.c
endif
ifneq (,$(filter gnrc_ipv6_whitelist,$(USEMODULE)))
  SRC += sc_whitelist.c
endif
ifneq (,$(filter gnrc_ipv6_blacklist,$(USEMODULE)))
  SRC += sc_blacklist.c
endif
ifneq (,$(filter gnrc_icmpv6_echo,$(USEMODULE)))
ifneq (,$(filter xtimer,$(USEMODULE)))
  SRC += sc_icmpv6_echo.c
endif
endif
ifneq (,$(filter gnrc_pktbuf_cmd,$(USEMODULE)))
    SRC += sc_gnrc_pktbuf.c
endif
ifneq (,$(filter gnrc_rpl,$(USEMODULE)))
    SRC += sc_gnrc_rpl.c
endif
ifneq (,$(filter gnrc_sixlowpan_ctx,$(USEMODULE)))
ifneq (,$(filter gnrc_ipv6_nib_6lbr,$(USEMODULE)))
    SRC += sc_gnrc_6ctx.c
endif
endif
ifneq (,$(filter saul_reg,$(USEMODULE)))
  SRC += sc_saul_reg.c
endif
ifneq (,$(filter ccn-lite-utils,$(USEMODULE)))
  SRC += sc_ccnl.c
endif
ifneq (,$(filter sntp,$(USEMODULE)))
  SRC += sc_sntp.c
endif
ifneq (,$(filter vfs,$(USEMODULE)))
  SRC += sc_vfs.c
endif
ifneq (,$(filter conn_can,$(USEMODULE)))
  SRC += sc_can.c
endif

ifneq (,$(filter periph_rtc,$(FEATURES_PROVIDED)))
  SRC += sc_rtc.c
endif

include $(RIOTBASE)/Makefile.base
