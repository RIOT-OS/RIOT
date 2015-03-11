MODULE = shell_commands

SRC = shell_commands.c sc_sys.c

ifneq (,$(filter config,$(USEMODULE)))
	SRC += sc_id.c
endif
ifneq (,$(filter transceiver,$(USEMODULE)))
	SRC += sc_transceiver.c
endif
ifneq (,$(filter cc110x_legacy_csma,$(USEMODULE)))
	ifeq (,$(filter transceiver,$(USEMODULE)))
		SRC += sc_cc110x_legacy_csma.c
	endif
endif
ifneq (,$(filter l2_ping,$(USEMODULE)))
	SRC += sc_l2_ping.c
endif
ifneq (,$(filter net_if,$(USEMODULE)))
	SRC += sc_net_if.c
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
ifneq (,$(filter rpl,$(USEMODULE)))
	SRC += sc_rpl.c
endif
ifneq (,$(filter sht11,$(USEMODULE)))
	SRC += sc_sht11.c
endif
ifneq (,$(filter lpc2387,$(USEMODULE)))
	SRC += sc_heap.c
endif
ifneq (,$(filter random,$(USEMODULE)))
	SRC += sc_mersenne.c
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
ifneq (,$(filter ng_netif,$(USEMODULE)))
	ifneq (,$(filter ng_netapi,$(USEMODULE)))
		ifneq (,$(filter ng_pktbuf,$(USEMODULE)))
			SRC += sc_netif.c
		endif
	endif
endif

# TODO
# Conditional building not possible at the moment due to
# https://github.com/RIOT-OS/RIOT/issues/2058
# The implementation is guarded in the source file instead, this
# should be changed once the issue has been resolved
SRC += sc_rtc.c

include $(RIOTBASE)/Makefile.base
