SRC = $(wildcard *.c)
BINDIR = $(RIOTBOARD)/$(BOARD)/bin/
OBJ = $(BINDIR)native-led.o
ifneq (,$(findstring ltc4150,$(USEMODULE)))
	OBJ += $(BINDIR)native-ltc4150.o
endif
ifneq (,$(findstring uart0,$(USEMODULE)))
	OBJ += $(BINDIR)native-uart0.o
endif
DEP = $(SRC:%.c=$(BINDIR)%.d)

INCLUDES += -I$(RIOTBOARD)/native/include/

$(BINDIR)native_drivers.a: $(OBJ)
	@$(AR) rcs $(BINDIR)${ARCH} $(OBJ)

# pull in dependency info for *existing* .o files
-include $(OBJ:.o=.d)

# compile and generate dependency info
$(BINDIR)%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) $(BOARDINCLUDE) $(PROJECTINCLUDE) $(CPUINCLUDE) -c $*.c -o $(BINDIR)$*.o
	@$(CC) $(CFLAGS) $(INCLUDES) $(BOARDINCLUDE) $(PROJECTINCLUDE) $(CPUINCLUDE) -MM $*.c > $(BINDIR)$*.d
	@printf "$(BINDIR)" | cat - $(BINDIR)$*.d > /tmp/riot_out && mv /tmp/riot_out $(BINDIR)$*.d

# remove compilation products

clean:
	rm -f $(OBJ) $(DEP)

