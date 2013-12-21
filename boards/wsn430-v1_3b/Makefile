MODULE =$(BOARD)_base

include $(RIOTBOARD)/$(BOARD)/Makefile.include

DIRS = $(RIOTBOARD)/wsn430-common

all: $(BINDIR)$(MODULE).a
	@for i in $(DIRS) ; do "$(MAKE)" -C $$i ; done ;

include $(RIOTBASE)/Makefile.base

clean::
	@for i in $(DIRS) ; do "$(MAKE)" -C $$i clean ; done ;
