MODULE = cpu_common
DIRS = periph pm

# build one of the radio drivers, if enabled
ifneq (,$(filter nrfmin,$(USEMODULE)))
  DIRS += radio/nrfmin
endif

include $(RIOTBASE)/Makefile.base
