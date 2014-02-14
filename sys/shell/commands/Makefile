SRC = shell_commands.c sc_id.c sc_sys.c

ifneq (,$(findstring transceiver,$(USEMODULE)))
	SRC += sc_transceiver.c
endif
ifneq (,$(findstring cc110x,$(USEMODULE)))
	ifeq (,$(findstring transceiver,$(USEMODULE)))
		SRC += sc_cc1100.c
	endif
endif
ifneq (,$(findstring mci,$(USEMODULE)))
	SRC += sc_disk.c
endif
ifneq (,$(findstring ltc4150,$(USEMODULE)))
	SRC += sc_ltc4150.c
endif
ifneq (,$(findstring ps,$(USEMODULE)))
	SRC += sc_ps.c
endif
ifneq (,$(findstring rtc,$(USEMODULE)))
	SRC += sc_rtc.c
endif
ifneq (,$(findstring sht11,$(USEMODULE)))
	SRC += sc_sht11.c
endif
ifneq (,$(findstring lpc_common,$(USEMODULE)))
	SRC += sc_heap.c
endif
ifneq (,$(findstring random,$(USEMODULE)))
	SRC += sc_mersenne.c
endif

OBJ = $(SRC:%.c=$(BINDIR)%.o)
DEP = $(SRC:%.c=$(BINDIR)%.d)

MODULE =shell_commands

include $(RIOTBASE)/Makefile.base
