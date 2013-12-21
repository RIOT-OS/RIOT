SRC = $(wildcard *.c)
OBJ = $(SRC:%.c=$(BINDIR)%.o)## defines
DEP = $(SRC:%.c=$(BINDIR)%.d)
export ARCH = $(BOARD)_base.a

INCLUDES += -I$(RIOTBOARD)/$(BOARD)/drivers/include
INCLUDES += -I$(RIOTBOARD)/$(BOARD)/include
INCLUDES += -I$(RIOTCPU)/arm_common/include
INCLUDES += -I$(RIOTCPU)/$(CPU)/include

.PHONY: $(BINDIR)/$(ARCH)

all: $(BINDIR)$(ARCH)
	$(MAKE) -C drivers

$(BINDIR)$(ARCH): $(OBJ)
	$(AR) rcs $(BINDIR)$(ARCH) $(OBJ)

# pull in dependency info for *existing* .o files
-include $(OBJ:.o=.d)

# compile and generate dependency info
$(BINDIR)%.o: %.c
	mkdir -p $(BINDIR)
	$(CC) $(CFLAGS) $(CFLAGS_MTHUMB) $(INCLUDES) $(BOARDINCLUDE) $(PROJECTINCLUDE) $(CPUINCLUDE) -c $*.c -o $(BINDIR)$*.o
	$(CC) $(CFLAGS) $(CFLAGS_MTHUMB) $(INCLUDES) $(BOARDINCLUDE) $(PROJECTINCLUDE) $(CPUINCLUDE) -MM $*.c > $(BINDIR)$*.d
	@printf "$(BINDIR)"|cat - $(BINDIR)$*.d > /tmp/riot_out && mv /tmp/riot_out $(BINDIR)$*.d

# remove compilation products
clean:
	"$(MAKE)" -C drivers clean
	rm -f $(BINDIR)$(ARCH) $(OBJ) $(DEP)
	@if [ -d $(BINDIR) ] ; \
	then rm -rf $(BINDIR) ; \
	fi

