MODULE =$(BOARD)_base

DIRS = drivers

all: $(BINDIR)$(MODULE).a
	@for i in $(DIRS) ; do "$(MAKE)" -C $$i ; done ;

include $(RIOTBASE)/Makefile.base

clean::
	@for i in $(DIRS) ; do "$(MAKE)" -C $$i clean ; done ;
