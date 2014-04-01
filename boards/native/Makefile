MODULE =$(BOARD)_base

DIRS = drivers

all: $(BINDIR)$(MODULE).a
	@for i in $(DIRS) ; do "$(MAKE)" -C $$i || exit 1; done ;

include $(RIOTBASE)/Makefile.base

$(BINDIR)%.o: %.c
	$(AD)$(CC) $(CFLAGS) $(NATIVEINCLUDES) -c $*.c -o $(BINDIR)$*.o
	$(AD)$(CC) $(CFLAGS) $(NATIVEINCLUDES) -MM $*.c |\
		sed -e "1s|^|$(BINDIR)|" > $(BINDIR)$*.d

clean::
	@for i in $(DIRS) ; do "$(MAKE)" -C $$i clean || exit 1; done ;
