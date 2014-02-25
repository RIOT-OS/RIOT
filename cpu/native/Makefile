MODULE = cpu

EXCLUDES := -I$(RIOTBASE)/sys/posix/%

DIRS =
ifneq (,$(filter rtc,$(USEMODULE)))
	DIRS += rtc
endif
ifneq (,$(filter nativenet,$(USEMODULE)))
	DIRS += net
endif

all: $(BINDIR)$(MODULE).a
	@for i in $(DIRS) ; do "$(MAKE)" -C $$i ; done ;

include $(RIOTBASE)/Makefile.base

$(BINDIR)%.o: %.c
	$(AD)$(CC) $(CFLAGS) $(filter-out $(EXCLUDES),$(INCLUDES)) $(BOARDINCLUDE) $(PROJECTINCLUDE) $(CPUINCLUDE) -c $*.c -o $(BINDIR)$*.o
	@$(CC) $(CFLAGS) $(INCLUDES) $(BOARDINCLUDE) $(PROJECTINCLUDE) $(CPUINCLUDE) -MM $*.c > $(BINDIR)$*.d
	@# prepend path to dependency info file
	$(AD)sed -i -e "1s|^|$(BINDIR)|" $(BINDIR)$*.d


clean::
	@for i in $(DIRS) ; do "$(MAKE)" -C $$i clean ; done ;
