MODULE = board

ifneq (,$(filter boards_common_adafruit-nrf52-bootloader,$(USEMODULE)))
  # add the adafruit-nrf52-bootloader directory to the build
  DIRS += $(RIOTBOARD)/common/adafruit-nrf52-bootloader
endif

include $(RIOTBASE)/Makefile.base
