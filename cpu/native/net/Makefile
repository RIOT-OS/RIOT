INCLUDES += -I$(RIOTBASE)/core/include -I$(RIOTBASE)/sys/include

EXCLUDES := -I$(RIOTBASE)/sys/posix/%

MODULE = nativenet

include $(MAKEBASE)/Makefile.base

$(BINDIR)%.o: %.c
	$(CC) $(CFLAGS) $(filter-out $(EXCLUDES),$(INCLUDES)) $(BOARDINCLUDE) $(PROJECTINCLUDE) $(CPUINCLUDE) -c $*.c -o $(BINDIR)$*.o
	@$(CC) $(CFLAGS) $(INCLUDES) $(BOARDINCLUDE) $(PROJECTINCLUDE) $(CPUINCLUDE) -MM $*.c > $(BINDIR)$*.d
	@printf "$(BINDIR)" | cat - $(BINDIR)$*.d > /tmp/riot_out && mv /tmp/riot_out $(BINDIR)$*.d

