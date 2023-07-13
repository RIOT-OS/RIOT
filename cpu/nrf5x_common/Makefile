MODULE = cpu_common
DIRS = periph

ifneq (,$(filter nrf_shared_serial_irq,$(USEMODULE)))
DIRS += shared_irq
endif
# build one of the radio drivers, if enabled
ifneq (,$(filter nrfble,$(USEMODULE)))
  DIRS += radio/nrfble
endif
ifneq (,$(filter nrfmin,$(USEMODULE)))
  DIRS += radio/nrfmin
endif

include $(RIOTBASE)/Makefile.base
