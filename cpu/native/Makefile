MODULE = cpu

DIRS += periph
DIRS += drivers
ifneq (,$(filter rtc,$(USEMODULE)))
	DIRS += rtc
endif
ifneq (,$(filter nativenet,$(USEMODULE)))
	DIRS += net
endif

include $(RIOTBASE)/Makefile.base

INCLUDES = $(NATIVEINCLUDES)
