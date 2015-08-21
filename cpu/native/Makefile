MODULE = cpu

DIRS += periph

ifneq (,$(filter netdev2_tap,$(USEMODULE)))
	DIRS += netdev2_tap
endif

include $(RIOTBASE)/Makefile.base

INCLUDES = $(NATIVEINCLUDES)
