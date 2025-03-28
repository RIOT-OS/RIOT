# add default board_init()
DIRS += $(RIOTBOARD)/common/init


# SORT THIS ALPHABETICALLY BY COMMON BOARD NAME!
ifneq (,$(filter boards_common_adafruit-nrf52-bootloader,$(USEMODULE)))
  DIRS += $(RIOTBOARD)/common/adafruit-nrf52-bootloader
endif
ifneq (,$(filter boards_common_seeedstudio-xiao-nrf52840,$(USEMODULE)))
  DIRS += $(RIOTBOARD)/common/seeedstudio-xiao-nrf52840
endif

include $(RIOTBASE)/Makefile.base
