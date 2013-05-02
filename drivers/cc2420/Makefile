INCLUDES = -I$(RIOTBASE)/sys/include -I../../net -I../include -I../../lib -I../../../.. -I../../../cpu/ -I$(RIOTBASE)/core/include -I../../ -Iinclude/ -I$(RIOTBASE)/sys/net/ieee802154/
MODULE =cc2420

DIRS =

all: $(BINDIR)$(MODULE).a
	@for i in $(DIRS) ; do $(MAKE) -C $$i ; done ;

include $(RIOTBASE)/Makefile.base

clean::
	@for i in $(DIRS) ; do $(MAKE) -C $$i clean ; done ;

