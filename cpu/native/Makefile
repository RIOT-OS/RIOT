MODULE = cpu

DIRS += periph
DIRS += vfs

ifneq (,$(filter netdev2_tap,$(USEMODULE)))
	DIRS += netdev2_tap
endif
ifneq (,$(filter mtd_native,$(USEMODULE)))
	DIRS += mtd
endif

include $(RIOTBASE)/Makefile.base

INCLUDES = $(NATIVEINCLUDES)
