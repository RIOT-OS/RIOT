INCLUDES = -I$(RIOTBASE)/sys/include -I$(RIOTBASE)/sys/net -I$(RIOTBASE)/core/include -Iinclude/ -I$(RIOTBASE)/sys/net/ieee802154/
MODULE =at86rf231

DIRS =

all: $(BINDIR)$(MODULE).a
	@for i in $(DIRS) ; do $(MAKE) -C $$i ; done ;

include $(RIOTBASE)/Makefile.base

clean::
	@for i in $(DIRS) ; do $(MAKE) -C $$i clean ; done ;

