MODULE = shell_commands

SRC = shell_commands.c sc_sys.c

ifneq (,$(filter config,$(USEMODULE)))
	SRC += sc_id.c
endif
ifneq (,$(filter transceiver,$(USEMODULE)))
	SRC += sc_transceiver.c
endif
ifneq (,$(filter cc110x,$(USEMODULE)))
	ifeq (,$(filter transceiver,$(USEMODULE)))
		SRC += sc_cc1100.c
	endif
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
ifneq (,$(filter rtc,$(USEMODULE)))
	SRC += sc_rtc.c
endif
ifneq (,$(filter sht11,$(USEMODULE)))
	SRC += sc_sht11.c
endif
ifneq (,$(filter lpc_common,$(USEMODULE)))
	SRC += sc_heap.c
endif
ifneq (,$(filter random,$(USEMODULE)))
	SRC += sc_mersenne.c
endif
ifneq (,$(filter gtsp,$(USEMODULE)))
	SRC += sc_gtsp.c
endif
ifneq (,$(filter ftsp,$(USEMODULE)))
	SRC += sc_ftsp.c
endif
ifneq (,$(filter pulsesync,$(USEMODULE)))
	SRC += sc_pulsesync.c
endif
ifeq ($(CPU),x86)
	SRC += sc_x86_lspci.c
endif

include $(RIOTBASE)/Makefile.base
