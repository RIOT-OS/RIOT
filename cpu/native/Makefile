MODULE = cpu

DIRS += periph
ifneq (,$(filter nativenet,$(USEMODULE)))
	DIRS += net
endif
ifneq (,$(filter ng_tapnet,$(USEMODULE)))
	DIRS += ng_tapnet
endif

include $(RIOTBASE)/Makefile.base

INCLUDES = $(NATIVEINCLUDES)
