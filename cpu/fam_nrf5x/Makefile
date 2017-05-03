MODULE = fam_nrf5x
DIRS = periph $(RIOTCPU)/arch_cortexm

# build one of the radio drivers, if enabled
ifneq (,$(filter nrfmin,$(USEMODULE)))
    DIRS += radio/nrfmin
endif

include $(RIOTBASE)/Makefile.base
