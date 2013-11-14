export ARCH = chronos_base.a

SRC = $(wildcard *.c)
BINDIR = $(RIOTBOARD)/$(BOARD)/bin/
OBJ = $(SRC:%.c=$(BINDIR)%.o)
DEP = $(SRC:%.c=$(BINDIR)%.d)

INCLUDES += -I$(RIOTBOARD)/$(BOARD)/include -I$(RIOTBASE)/sys/lib -I$(RIOTBASE)/sys/drivers/include
INCLUDES += -I$(RIOTBASE)/sys/drivers/cc110x_ng/include
INCLUDES += -I$(RIOTBASE)/core/include
INCLUDES += -I$(RIOTCPU)/msp430-common/include/
INCLUDES += -I$(RIOTCPU)/cc430/include/

all: $(BINDIR)$(ARCH)
	$(MAKE) -C drivers

$(BINDIR)$(ARCH): $(OBJ)
	mkdir -p $(BINDIR)
	$(AR) rcs $(BINDIR)$(ARCH) $(OBJ)

# pull in dependency info for *existing* .o files
-include $(OBJ:.o=.d)

# compile and generate dependency info
$(BINDIR)%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) $(BOARDINCLUDE) $(PROJECTINCLUDE) $(CPUINCLUDE) -c $*.c -o $(BINDIR)$*.o
	$(CC) $(CFLAGS) $(INCLUDES) $(BOARDINCLUDE) $(PROJECTINCLUDE) $(CPUINCLUDE) -MM $*.c > $(BINDIR)$*.d
	@printf "$(BINDIR)"|cat - $(BINDIR)$*.d > /tmp/fw_out && mv /tmp/fw_out $(BINDIR)$*.d

# remove compilation products
clean:
	$(MAKE) -C drivers clean
	rm -f $(OBJ) $(DEP)
	rm -f $(BINDIR)$(ARCH)
