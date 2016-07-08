MODULE = shell_commands

SRC = shell_commands.c sc_sys.c

ifneq (,$(filter mci,$(USEMODULE)))
  SRC += sc_disk.c
endif
ifneq (,$(filter ps,$(USEMODULE)))
  SRC += sc_ps.c
endif
ifneq (,$(filter sht11,$(USEMODULE)))
  SRC += sc_sht11.c
endif
ifneq (,$(filter lpc2387,$(USEMODULE)))
  SRC += sc_heap.c
endif
ifneq (,$(filter random,$(USEMODULE)))
  SRC += sc_random.c
endif
ifeq ($(CPU),x86)
  SRC += sc_x86_lspci.c
endif
ifneq (,$(filter at30tse75x,$(USEMODULE)))
    SRC += sc_at30tse75x.c
endif
ifneq (,$(filter gnrc_netif,$(USEMODULE)))
  SRC += sc_netif.c
endif
ifneq (,$(filter fib,$(USEMODULE)))
  SRC += sc_fib.c
endif
ifneq (,$(filter gnrc_ipv6_nc,$(USEMODULE)))
  SRC += sc_ipv6_nc.c
endif
ifneq (,$(filter gnrc_ipv6_whitelist,$(USEMODULE)))
  SRC += sc_whitelist.c
endif
ifneq (,$(filter gnrc_ipv6_blacklist,$(USEMODULE)))
  SRC += sc_blacklist.c
endif
# The ping command in sc_icmpv6_echo requires xtimer, too. However, this is
# implicitly pulled in by gnrc_ipv6_netif (which is a dependency of gnrc_ipv6)
# already.
ifneq (,$(filter gnrc_icmpv6_echo,$(USEMODULE)))
  SRC += sc_icmpv6_echo.c
endif
ifneq (,$(filter gnrc_rpl,$(USEMODULE)))
    SRC += sc_gnrc_rpl.c
endif
ifneq (,$(filter gnrc_sixlowpan_ctx,$(USEMODULE)))
ifneq (,$(filter gnrc_sixlowpan_nd_border_router,$(USEMODULE)))
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

# TODO
# Conditional building not possible at the moment due to
# https://github.com/RIOT-OS/RIOT/issues/2058
# The implementation is guarded in the source file instead, this
# should be changed once the issue has been resolved
SRC += sc_rtc.c

include $(RIOTBASE)/Makefile.base
