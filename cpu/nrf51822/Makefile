# define the module that is build
MODULE = cpu

# add a list of subdirectories, that should also be build
DIRS = periph $(CORTEX_COMMON)

# also build the radio driver when enabled
ifneq (,$(filter nrf51,$(USEMODULE)))
    DIRS += nrf51
endif

include $(RIOTBASE)/Makefile.base
