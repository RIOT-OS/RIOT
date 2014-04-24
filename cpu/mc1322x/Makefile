MODULE =cpu

DIRS = $(RIOTCPU)/arm_common
ifneq (,$(filter mc1322x_adc,$(USEMODULE)))
	DIRS += adc
endif
ifneq (,$(filter mc1322x_asm,$(USEMODULE)))
	DIRS += asm
endif
ifneq (,$(filter mc1322x,$(USEMODULE)))
	DIRS += maca
endif

all: $(BINDIR)$(MODULE).a
	@for i in $(DIRS) ; do "$(MAKE)" -C $$i || exit 1; done ;

include $(RIOTBASE)/Makefile.base
