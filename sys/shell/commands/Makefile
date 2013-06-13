SRC = shell_commands.c sc_id.c
INCLUDES = -I../../../core/include -I../../include -I../../../drivers/include/

ifneq (,$(findstring cc110x_ng,$(USEMODULE)))
	INCLUDES += -I$(RIOTBASE)/drivers/cc110x_ng/include/
	SRC += sc_cc110x_ng.c
endif
ifneq (,$(findstring cc110x,$(USEMODULE)))
	INCLUDES += -I$(RIOTBASE)/drivers/cc110x/
	SRC += sc_cc1100.c
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

$(BINDIR)$(MODULE).a: $(OBJ)
	$(AR) rc $(BINDIR)$(MODULE).a $(OBJ)

# pull in dependency info for *existing* .o files
-include $(OBJ:.o=.d)

# compile and generate dependency info
$(BINDIR)%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $*.c -o $(BINDIR)$*.o
	$(CC) $(CFLAGS) $(INCLUDES) -MM $*.c > $(BINDIR)$*.d
	@printf "$(BINDIR)"|cat - $(BINDIR)$*.d > /tmp/riot_out && mv /tmp/riot_out $(BINDIR)$*.d

# remove compilation products
clean::
	rm -f $(BINDIR)$(MODULE).a $(OBJ) $(DEP) $(ASMOBJ)
