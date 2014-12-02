MODULE =cpu

DIRS = $(RIOTCPU)/arm7_common
ifneq (,$(filter mc1322x_adc,$(USEMODULE)))
	DIRS += adc
endif
ifneq (,$(filter mc1322x_asm,$(USEMODULE)))
	DIRS += asm
endif
ifneq (,$(filter mc1322x,$(USEMODULE)))
	DIRS += maca
endif

include $(RIOTBASE)/Makefile.base
