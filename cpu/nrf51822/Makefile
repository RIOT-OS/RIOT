# define the module that is build
MODULE = cpu

# add a list of subdirectories, that should also be build
DIRS = periph $(CORTEX_COMMON)
ifneq (,$(filter ng_ble,$(USEMODULE)))
	DIRS += ng_ble
endif

include $(RIOTBASE)/Makefile.base
