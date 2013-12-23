MODULE =$(BOARD)_base
INCLUDES += -I$(RIOTBASE)/drivers/cc110x

DIRS = $(RIOTBOARD)/msba2-common 

all: $(BINDIR)$(MODULE).a
	@for i in $(DIRS) ; do "$(MAKE)" -C $$i ; done ;

include $(RIOTBASE)/Makefile.base

clean::
	@for i in $(DIRS) ; do "$(MAKE)" -C $$i clean ; done ;
