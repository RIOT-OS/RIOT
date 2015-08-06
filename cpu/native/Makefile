MODULE = cpu

DIRS += periph

ifneq (,$(filter ng_nativenet,$(USEMODULE)))
	DIRS += ng_net
endif

include $(RIOTBASE)/Makefile.base

INCLUDES = $(NATIVEINCLUDES)
