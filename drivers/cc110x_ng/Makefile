INCLUDES = -I$(RIOTBASE)/sys/include -I../../net -I../include -I../../lib -I../../../.. -I../../../cpu/ -I$(RIOTBASE)/core/include -I../../ -Iinclude/
MODULE =cc110x_ng

DIRS =
ifneq (,$(findstring msb-430h,$(BOARD)))
	DIRS += spi 
endif
ifneq (,$(findstring msba2,$(BOARD)))
	DIRS += spi 
endif
ifneq (,$(findstring native,$(BOARD)))
	DIRS += spi 
endif

all: $(BINDIR)$(MODULE).a
	@for i in $(DIRS) ; do $(MAKE) -C $$i ; done ;

include $(RIOTBASE)/Makefile.base

clean::
	@for i in $(DIRS) ; do $(MAKE) -C $$i clean ; done ;

