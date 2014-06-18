MODULE =$(BOARD)_base

DIRS = drivers

include $(RIOTBASE)/Makefile.base

$(BINDIR)$(MODULE)/%.o: %.c
	@mkdir -p $(BINDIR)$(MODULE)
	$(AD)$(CC) $(CFLAGS) $(NATIVEINCLUDES) -c $*.c -o $(BINDIR)$(MODULE)/$*.o
	$(AD)$(CC) $(CFLAGS) $(NATIVEINCLUDES) -MM $*.c |\
		sed -e "1s|^|$(BINDIR)$(MODULE)/|" > $(BINDIR)$(MODULE)/$*.d
