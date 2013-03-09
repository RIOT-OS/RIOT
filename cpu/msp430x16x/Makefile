MODULE =cpu

INCLUDES = -I../include -I$(RIOTBASE)/core/include
DIRS =

all: $(BINDIR)$(MODULE).a
	@for i in $(DIRS) ; do $(MAKE) -C $$i ; done ;

include $(RIOTBASE)/makefile.base

clean::
	@for i in $(DIRS) ; do $(MAKE) -C $$i clean ; done ;

