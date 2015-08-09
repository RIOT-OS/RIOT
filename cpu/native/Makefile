MODULE = cpu

DIRS += periph

ifneq (,$(filter dev_eth_tap,$(USEMODULE)))
	DIRS += dev_eth_tap
endif

include $(RIOTBASE)/Makefile.base

INCLUDES = $(NATIVEINCLUDES)
