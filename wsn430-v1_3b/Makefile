SRC = $(wildcard *.c)
BINDIR = bin/
OBJ = $(SRC:%.c=$(BINDIR)%.o)## defines
export ARCH = wsn430-v1_3b_base.a

DEP = $(SRC:%.c=$(BINDIR)%.d)

INCLUDES += -I${RIOTBASE}/core/include/
INCLUDES += -Iinclude/ -I$(RIOTBOARD)/wsn430-common/include/
INCLUDES += -I$(RIOTBASE)/cpu/msp430-common/include/ -I$(RIOTBASE)/cpu/msp430x16x/include/
INCLUDES += -I$(RIOTBASE)/drivers/cc110x_ng/include/ -I$(RIOTBASE)/sys/include
#INCLUDES += -I/usr/msp430/include/

all: $(BINDIR)$(ARCH)
	$(MAKE) -C ../wsn430-common

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
	$(MAKE) -C ../wsn430-common clean
	rm -f $(BINDIR)$(ARCH) $(OBJ) $(DEP)
	@if [ -d $(BINDIR) ] ; \
	then rmdir $(BINDIR) ; \
	fi 