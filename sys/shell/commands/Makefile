SRC = shell_commands.c sc_id.c sc_heap.c
INCLUDES = -I../../../core/include -I../../include -I../../../drivers/include/

ifneq (,$(findstring cc110x_ng,$(USEMODULE)))
	INCLUDES += -I$(RIOTBASE)/drivers/cc110x_ng/include/
	SRC += sc_cc110x_ng.c
endif
ifneq (,$(findstring cc2420,$(USEMODULE)))
	INCLUDES += -I$(RIOTBASE)/drivers/cc2420/include/ -I$(RIOTBASE)/sys/net/include
	SRC += sc_cc2420.c
endif
ifneq (,$(findstring cc110x,$(USEMODULE)))
	INCLUDES += -I$(RIOTBASE)/drivers/cc110x/
	SRC += sc_cc1100.c
endif
ifneq (,$(findstring nativenet,$(USEMODULE)))
	INCLUDES += -I$(RIOTBASE)/cpu/native/include
	SRC += sc_nativenet.c
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

OBJ = $(SRC:%.c=$(BINDIR)%.o)
DEP = $(SRC:%.c=$(BINDIR)%.d)

MODULE =shell_commands

include $(RIOTBASE)/Makefile.base

