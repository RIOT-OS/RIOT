MODULE = cpu

DIRS += periph
DIRS += vfs

ifneq (,$(filter netdev2_tap,$(USEMODULE)))
	DIRS += netdev2_tap
endif

include $(RIOTBASE)/Makefile.base

INCLUDES = $(NATIVEINCLUDES)
