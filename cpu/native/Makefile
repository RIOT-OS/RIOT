MODULE = cpu

DIRS =
ifneq (,$(filter rtc,$(USEMODULE)))
	DIRS += rtc
endif
ifneq (,$(filter nativenet,$(USEMODULE)))
	DIRS += net
endif

all: $(BINDIR)$(MODULE).a
	@for i in $(DIRS) ; do "$(MAKE)" -C $$i || exit 1; done ;

include $(RIOTBASE)/Makefile.base

$(BINDIR)$(MODULE)/%.o: %.c
	@mkdir -p $(BINDIR)$(MODULE)
	$(AD)$(CC) $(CFLAGS) $(NATIVEINCLUDES) -c $*.c -o $(BINDIR)$(MODULE)/$*.o
	$(AD)$(CC) $(CFLAGS) $(NATIVEINCLUDES) -MM $*.c |\
		sed -e "1s|^|$(BINDIR)$(MODULE)/|" > $(BINDIR)$(MODULE)/$*.d
