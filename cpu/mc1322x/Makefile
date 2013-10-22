MODULE =cpu

DIRS =
ifneq (,$(findstring mc1322x_adc,$(USEMODULE)))
  DIRS += adc
endif
ifneq (,$(findstring mc1322x_asm,$(USEMODULE)))
	DIRS += asm
endif

all: $(BINDIR)$(MODULE).a
	@for i in $(DIRS) ; do "$(MAKE)" -C $$i ; done ;

include $(RIOTBASE)/Makefile.base

clean::
	@for i in $(DIRS) ; do "$(MAKE)" -C $$i clean ; done ;

