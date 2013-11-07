INCLUDES = -I$(RIOTBASE)/sys/include  -I$(RIOTBASE)/core/include -Iinclude/ -I$(RIOTBASE)/sys/net/include
MODULE =cc2420

DIRS =

all: $(BINDIR)$(MODULE).a
	@for i in $(DIRS) ; do "$(MAKE)" -C $$i ; done ;

include $(RIOTBASE)/Makefile.base

clean::
	@for i in $(DIRS) ; do "$(MAKE)" -C $$i clean ; done ;

