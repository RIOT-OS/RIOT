MODULE = shell_commands

SRC = shell_commands.c sc_sys.c

ifneq (,$(filter config,$(USEMODULE)))
  SRC += sc_id.c
endif
ifneq (,$(filter mci,$(USEMODULE)))
  SRC += sc_disk.c
endif
ifneq (,$(filter ltc4150,$(USEMODULE)))
  SRC += sc_ltc4150.c
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
ifneq (,$(filter isl29020,$(USEMODULE)))
  SRC += sc_isl29020.c
endif
ifneq (,$(filter lps331ap,$(USEMODULE)))
  SRC += sc_lps331ap.c
endif
ifneq (,$(filter l3g4200d,$(USEMODULE)))
  SRC += sc_l3g4200d.c
endif
ifneq (,$(filter lsm303dlhc,$(USEMODULE)))
  SRC += sc_lsm303dlhc.c
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
ifneq (,$(filter gnrc_icmpv6_echo xtimer,$(USEMODULE)))
  SRC += sc_icmpv6_echo.c
endif
ifneq (,$(filter gnrc_zep,$(USEMODULE)))
  ifneq (,$(filter ipv6_addr,$(USEMODULE)))
    SRC += sc_zep.c
  endif
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

# TODO
# Conditional building not possible at the moment due to
# https://github.com/RIOT-OS/RIOT/issues/2058
# The implementation is guarded in the source file instead, this
# should be changed once the issue has been resolved
SRC += sc_rtc.c

include $(RIOTBASE)/Makefile.base
