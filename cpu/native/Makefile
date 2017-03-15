MODULE = cpu

DIRS += periph
DIRS += vfs

ifneq (,$(filter netdev_tap,$(USEMODULE)))
	DIRS += netdev_tap
endif

include $(RIOTBASE)/Makefile.base

INCLUDES = $(NATIVEINCLUDES)
