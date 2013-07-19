MODULE =cpu

INCLUDES += -Iinclude -Idrivers/include -I../arm_common/include -I../../sys/lib -I../../sys/drivers/include -I../../core/include -I../../sys/include -I../../hal/include -I../../.. 
INCLUDES += -I$(RIOTBASE)/drivers/cc110x_ng/include
DIRS =

all: $(BINDIR)$(MODULE).a
	@for i in $(DIRS) ; do "$(MAKE)" -C $$i ; done ;

include $(RIOTBASE)/Makefile.base

clean::
	@for i in $(DIRS) ; do "$(MAKE)" -C $$i clean ; done ;


