SRC = $(wildcard *.c)
BINDIR = bin/
OBJ = $(SRC:%.c=$(BINDIR)%.o)## defines

DEP = $(SRC:%.c=$(BINDIR)%.d)

INCLUDES += -Iinclude -I$(RIOTBASE)/sys/lib -I$(RIOTBASE)/sys/drivers/include
INCLUDES += -I$(RIOTBASE)/sys/drivers/cc110x_ng/include
INCLUDES += -I$(RIOTBASE)/core/include
INCLUDES += -I$(RIOTBASE)/cpu/msp430-common/include/
INCLUDES += -I$(RIOTBASE)/cpu/cc430/include/
#INCLUDES += -I/usr/msp430/include/

all: $(BINDIR)chronos_base.a 
	$(MAKE) -C drivers

$(BINDIR)chronos_base.a: $(OBJ)
	$(AR) rcs $(BINDIR)chronos_base.a $(OBJ)

# pull in dependency info for *existing* .o files
-include $(OBJ:.o=.d)

# compile and generate dependency info
$(BINDIR)%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) $(BOARDINCLUDE) $(PROJECTINCLUDE) $(CPUINCLUDE) -c $*.c -o $(BINDIR)$*.o
	$(CC) $(CFLAGS) $(INCLUDES) $(BOARDINCLUDE) $(PROJECTINCLUDE) $(CPUINCLUDE) -MM $*.c > $(BINDIR)$*.d
	@printf "$(BINDIR)"|cat - $(BINDIR)$*.d > /tmp/fw_out && mv /tmp/fw_out $(BINDIR)$*.d

# remove compilation products
clean:
	rm -f $(BINDIR)chronos_base.a $(OBJ) $(DEP)
	$(MAKE) -C drivers clean



