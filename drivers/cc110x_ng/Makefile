MODULE =cc110x_ng

DIRS =
ifneq (,$(filter msb-430h,$(BOARD)))
	DIRS += spi
endif
ifneq (,$(filter msba2,$(BOARD)))
	DIRS += spi
endif
ifneq (,$(filter wsn430-v1_3b,$(BOARD)))
	DIRS += spi
endif

all: $(BINDIR)$(MODULE).a
	@for i in $(DIRS) ; do "$(MAKE)" -C $$i ; done ;

include $(RIOTBASE)/Makefile.base

clean::
	@for i in $(DIRS) ; do "$(MAKE)" -C $$i clean ; done ;
