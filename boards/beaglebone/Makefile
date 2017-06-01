
# tell the Makefile.base which module to build
MODULE = $(BOARD)_base

# add a list of board specific subdirectories that should also be build
DIRS = starterware

.PHONY: all clean

all: $(BINDIR)$(MODULE).a
	@for i in $(DIRS) ; do $(MAKE) -C $$i ; done ;

include $(RIOTBASE)/Makefile.base

clean::
	@for i in $(DIRS) ; do $(MAKE) -C $$i clean ; done ;
