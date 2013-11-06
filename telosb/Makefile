SRC = $(wildcard *.c)
BINDIR = bin/
OBJ = $(SRC:%.c=$(BINDIR)%.o)## defines
export ARCH = telosb_base.a

DEP = $(SRC:%.c=$(BINDIR)%.d)

INCLUDES += -I${RIOTBOARD}/${BOARD}/include/
INCLUDES += -I${RIOTBASE}/core/include/
INCLUDES += -I$(RIOTBASE)/cpu/msp430-common/include/ -I$(RIOTBASE)/cpu/msp430x16x/include/
INCLUDES += -I$(RIOTBASE)/drivers/cc2420/include/ -I$(RIOTBASE)/sys/include
INCLUDES += -I$(RIOTBASE)/sys/net/include

all: $(BINDIR)$(ARCH)

$(BINDIR)$(ARCH): $(OBJ)
	$(AR) rcs $(BINDIR)$(ARCH) $(OBJ)

# pull in dependency info for *existing* .o files
-include $(OBJ:.o=.d)

# compile and generate dependency info
$(BINDIR)%.o: %.c
	mkdir -p $(BINDIR)
	$(CC) $(CFLAGS) $(INCLUDES) $(BOARDINCLUDE) $(PROJECTINCLUDE) $(CPUINCLUDE) -c $*.c -o $(BINDIR)$*.o
	$(CC) $(CFLAGS) $(INCLUDES) $(BOARDINCLUDE) $(PROJECTINCLUDE) $(CPUINCLUDE) -MM $*.c > $(BINDIR)$*.d
	@printf "$(BINDIR)"|cat - $(BINDIR)$*.d > /tmp/riot_out && mv /tmp/riot_out $(BINDIR)$*.d

# remove compilation products
clean:
	rm -f $(BINDIR)$(ARCH) $(OBJ) $(DEP)
	@if [ -d $(BINDIR) ] ; \
	then rmdir $(BINDIR) ; \
	fi
