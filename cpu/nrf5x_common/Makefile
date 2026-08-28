MODULE = cpu_common
DIRS = periph

# The clock handling of the nRF54L family differs too much from the older
# families and is not supported yet (it is only needed by the rtt, radio and
# usb drivers, which are not supported either)
ifeq (nrf54l,$(CPU_FAM))
  SRC = $(filter-out clock.c,$(wildcard *.c))
  # an empty SRC would be auto-filled with *.c again
  NO_AUTO_SRC = 1
endif

ifneq (,$(filter nrf_shared_serial_irq,$(USEMODULE)))
DIRS += shared_irq
endif
# build one of the radio drivers, if enabled
ifneq (,$(filter nrf802154,$(USEMODULE)))
  DIRS += radio/nrf802154
endif
ifneq (,$(filter nrfble,$(USEMODULE)))
  DIRS += radio/nrfble
endif
ifneq (,$(filter nrfmin,$(USEMODULE)))
  DIRS += radio/nrfmin
endif

include $(RIOTBASE)/Makefile.base
