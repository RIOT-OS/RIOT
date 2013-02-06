MODULE =cpu

INCLUDES = -Iinclude -Idrivers/include -I../arm_common/include -I../../sys/lib -I../../drivers/include -I../../core/include -I../../sys/include -I../../hal/include -I../../.. 
DIRS =

all: $(BINDIR)$(MODULE).a
	@for i in $(DIRS) ; do $(MAKE) -C $$i ; done ;

include $(RIOTBASE)/makefile.base

clean::
	@for i in $(DIRS) ; do $(MAKE) -C $$i clean ; done ;

