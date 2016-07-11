MODULE = cpu

DIRS += periph

ifneq (,$(filter netdev2_tap,$(USEMODULE)))
	DIRS += netdev2_tap
endif
ifneq (,$(filter mtdi_native,$(USEMODULE)))
	DIRS += mtdi
endif

include $(RIOTBASE)/Makefile.base

INCLUDES = $(NATIVEINCLUDES)
