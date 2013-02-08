SRC = $(wildcard *.c)
BINDIR = bin/
OBJ = $(SRC:%.c=$(BINDIR)%.o)## defines

DEP = $(SRC:%.c=$(BINDIR)%.d)

INCLUDES += -I$(RIOTBOARD)/msba2-common/include/
INCLUDES += -I$(RIOTBASE)/cpu/arm_common/include/
INCLUDES += -I$(RIOTBASE)/cpu/lpc2387/include/

all: $(BINDIR)msba2_base.a 
	$(MAKE) -C ../msba2-common

$(BINDIR)msba2_base.a: $(OBJ)
	$(AR) rcs $(BINDIR)msba2_base.a $(OBJ)

# pull in dependency info for *existing* .o files
-include $(OBJ:.o=.d)

# compile and generate dependency info
$(BINDIR)%.o: %.c
	mkdir $(BINDIR)
	$(CC) $(CFLAGS) $(INCLUDES) $(BOARDINCLUDE) $(PROJECTINCLUDE) $(CPUINCLUDE) -c $*.c -o $(BINDIR)$*.o
	$(CC) $(CFLAGS) $(INCLUDES) $(BOARDINCLUDE) $(PROJECTINCLUDE) $(CPUINCLUDE) -MM $*.c > $(BINDIR)$*.d
	@printf "$(BINDIR)"|cat - $(BINDIR)$*.d > /tmp/riot_out && mv /tmp/riot_out $(BINDIR)$*.d

# remove compilation products
clean:
	$(MAKE) -C ../msba2-common clean
	rm -f $(BINDIR)msba2_base.a $(OBJ) $(DEP)
	@if [ -d $(BINDIR) ] ; \
	then rmdir $(BINDIR) ; \
	fi 


