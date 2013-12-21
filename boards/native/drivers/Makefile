MODULE =$(BOARD)_base

include $(RIOTBOARD)/$(BOARD)/Makefile.include

all: $(BINDIR)$(MODULE).a
	@for i in $(DIRS) ; do "$(MAKE)" -C $$i ; done ;

include $(RIOTBASE)/Makefile.base
