SRC = $(wildcard *.c)
BINDIR = bin/
OBJ = $(SRC:%.c=$(BINDIR)%.o)## defines

DEP = $(SRC:%.c=$(BINDIR)%.d)

INCLUDES += -I$(RIOTBOARD)/msb-430-common/include/
INCLUDES += -I$(RIOTBASE)/cpu/msp430-common/include/
INCLUDES += -I$(RIOTBASE)/cpu/msp430x16x/include/
INCLUDES += -I$(RIOTBASE)/drivers/cc110x_ng/include/ -I$(RIOTBASE)/sys/include

all: $(BINDIR)msb-430_base.a 
	$(MAKE) -C ../msb-430-common

$(BINDIR)msb-430_base.a: $(OBJ)
	$(AR) rcs $(BINDIR)msb-430_base.a $(OBJ)

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
	$(MAKE) -C ../msb-430-common clean
	rm -f $(BINDIR)msb-430_base.a $(OBJ) $(DEP)
	@if [ -d $(BINDIR) ] ; \
	then rmdir $(BINDIR) ; \
	fi 


