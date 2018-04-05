MODULE = cpu_common
DIRS = periph

# build one of the radio drivers, if enabled
ifneq (,$(filter nrfble,$(USEMODULE)))
  DIRS += radio/nrfble
endif
ifneq (,$(filter nrfmin,$(USEMODULE)))
  DIRS += radio/nrfmin
endif

include $(RIOTBASE)/Makefile.base
