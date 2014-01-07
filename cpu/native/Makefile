MODULE = cpu

EXCLUDES := -I$(RIOTBASE)/sys/posix/%

DIRS =
ifneq (,$(findstring rtc,$(USEMODULE)))
	DIRS += rtc
endif
ifneq (,$(findstring nativenet,$(USEMODULE)))
	DIRS += net
endif

all: $(BINDIR)$(MODULE).a
	@for i in $(DIRS) ; do "$(MAKE)" -C $$i ; done ;

include $(RIOTBASE)/Makefile.base

$(BINDIR)%.o: %.c
	$(AD)$(CC) $(CFLAGS) $(filter-out $(EXCLUDES),$(INCLUDES)) $(BOARDINCLUDE) $(PROJECTINCLUDE) $(CPUINCLUDE) -c $*.c -o $(BINDIR)$*.o
	@$(CC) $(CFLAGS) $(INCLUDES) $(BOARDINCLUDE) $(PROJECTINCLUDE) $(CPUINCLUDE) -MM $*.c > $(BINDIR)$*.d
	@printf "$(BINDIR)" | cat - $(BINDIR)$*.d > /tmp/riot_out && mv /tmp/riot_out $(BINDIR)$*.d


clean::
	@for i in $(DIRS) ; do "$(MAKE)" -C $$i clean ; done ;

