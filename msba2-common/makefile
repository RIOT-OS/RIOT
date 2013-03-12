SRC = $(wildcard *.c)
BINDIR = $(RIOTBOARD)/$(BOARD)/bin/
OBJ = $(SRC:%.c=$(BINDIR)%.o)## defines
DEP = $(SRC:%.c=$(BINDIR)%.d)

INCLUDES += -I$(RIOTBOARD)/msba2-common/include/
INCLUDES += -I$(RIOTBASE)/cpu/arm_common/include/
INCLUDES += -I$(RIOTBASE)/cpu/lpc2387/include/
INCLUDES += -I$(RIOTBASE)/drivers/include/

.PHONY: $(BINDIR)msba2_common_base.a

all: $(BINDIR)msba2_common_base.a 
	$(MAKE) -C drivers

$(BINDIR)msba2_common_base.a: $(OBJ)
	$(AR) rcs $(BINDIR)$(ARCH) $(OBJ)

# pull in dependency info for *existing* .o files
-include $(OBJ:.o=.d)

# compile and generate dependency info
$(BINDIR)%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) $(BOARDINCLUDE) $(PROJECTINCLUDE) $(CPUINCLUDE) -c $*.c -o $(BINDIR)$*.o
	$(CC) $(CFLAGS) $(INCLUDES) $(BOARDINCLUDE) $(PROJECTINCLUDE) $(CPUINCLUDE) -MM $*.c > $(BINDIR)$*.d
	@printf "$(BINDIR)"|cat - $(BINDIR)$*.d > /tmp/riot_out && mv /tmp/riot_out $(BINDIR)$*.d

# remove compilation products
clean:
	${MAKE} -C drivers clean
	rm -f $(OBJ) $(DEP)

