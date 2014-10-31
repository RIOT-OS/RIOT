MODULE = cpu

DIRS += periph
ifneq (,$(filter nativenet,$(USEMODULE)))
	DIRS += net
endif

include $(RIOTBASE)/Makefile.base

INCLUDES = $(NATIVEINCLUDES)
