SRC = $(wildcard *.c)
BINDIR = $(RIOTBOARD)/$(BOARD)/bin/
OBJ = $(SRC:%.c=$(BINDIR)%.o)## defines
DEP = $(SRC:%.c=$(BINDIR)%.d)
export ARCH = mbed_lpc1768_base.a

INCLUDES += -Iinclude/
INCLUDES += -I$(RIOTCPU)/$(CPU)/include
INCLUDES += -I$(RIOTBASE)/core/include

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
	rm -f $(OBJ) $(DEP)
