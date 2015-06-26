# define the module that is build
MODULE = cpu

# add a list of subdirectories, that should also be build
DIRS = periph $(RIOTCPU)/cortexm_common

# build one of the radio drivers, if enabled
ifneq (,$(filter radio_nrfmin,$(USEMODULE)))
    DIRS += radio/nrfmin
endif

# build one of the radio drivers, if enabled
ifneq (,$(filter nrf51_ble,$(USEMODULE)))
    DIRS += radio/nrf51_ble
endif

include $(RIOTBASE)/Makefile.base
