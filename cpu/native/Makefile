MODULE = cpu

ifneq (,$(filter rtc,$(USEMODULE)))
	DIRS += rtc
endif
ifneq (,$(filter nativenet,$(USEMODULE)))
	DIRS += net
endif

include $(RIOTBASE)/Makefile.base

$(BINDIR)$(MODULE)/%.o: %.c
	@mkdir -p $(BINDIR)$(MODULE)
	$(AD)$(CC) $(CFLAGS) $(NATIVEINCLUDES) -c $*.c -o $(BINDIR)$(MODULE)/$*.o
	$(AD)$(CC) $(CFLAGS) $(NATIVEINCLUDES) -MM $*.c |\
		sed -e "1s|^|$(BINDIR)$(MODULE)/|" > $(BINDIR)$(MODULE)/$*.d
