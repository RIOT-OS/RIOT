# add default board_init()
DIRS += $(RIOTBOARD)/common/init


# SORT THIS ALPHABETICALLY BY COMMON BOARD NAME!
ifneq (,$(filter boards_common_adafruit-nrf52-bootloader,$(USEMODULE)))
  DIRS += $(RIOTBOARD)/common/adafruit-nrf52-bootloader
endif

include $(RIOTBASE)/Makefile.base
