MODULE = cpu

INCLUDES += -I$(RIOTCPU)/$(CPU)/include -I$(RIOTBASE)/core/include -I$(RIOTBASE)/drivers/cc110x_ng/include
DIRS =

all: $(BINDIR)$(MODULE).a
	@for i in $(DIRS) ; do "$(MAKE)" -C $$i ; done ;

include $(RIOTBASE)/Makefile.base

clean::
	@for i in $(DIRS) ; do "$(MAKE)" -C $$i clean ; done ;
