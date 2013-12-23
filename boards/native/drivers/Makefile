MODULE =$(BOARD)_base

all: $(BINDIR)$(MODULE).a
	@for i in $(DIRS) ; do "$(MAKE)" -C $$i ; done ;

include $(RIOTBASE)/Makefile.base
