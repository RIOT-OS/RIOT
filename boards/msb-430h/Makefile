MODULE =$(BOARD)_base

DIRS = $(RIOTBOARD)/msb-430-common 

INCLUDES += -I$(RIOTBASE)/drivers/cc110x_ng/include

all: $(BINDIR)$(MODULE).a
	@for i in $(DIRS) ; do "$(MAKE)" -C $$i ; done ;

include $(RIOTBASE)/Makefile.base

clean::
	@for i in $(DIRS) ; do "$(MAKE)" -C $$i clean ; done ;
