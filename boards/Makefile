# add default board_init()
DIRS += $(RIOTBOARD)/common/init


# SORT THIS ALPHABETICALLY BY COMMON BOARD NAME!
ifneq (,$(filter boards_common_adafruit-nrf52-bootloader,$(USEMODULE)))
  DIRS += $(RIOTBOARD)/common/adafruit-nrf52-bootloader
endif
ifneq (,$(filter boards_common_esp32,$(USEMODULE)))
  DIRS += $(RIOTBOARD)/common/esp32
endif
ifneq (,$(filter boards_common_esp32c3,$(USEMODULE)))
  DIRS += $(RIOTBOARD)/common/esp32c3
endif
ifneq (,$(filter boards_common_esp32c6,$(USEMODULE)))
  DIRS += $(RIOTBOARD)/common/esp32c6
endif
ifneq (,$(filter boards_common_esp32h2,$(USEMODULE)))
  DIRS += $(RIOTBOARD)/common/esp32h2
endif
ifneq (,$(filter boards_common_esp32s2,$(USEMODULE)))
  DIRS += $(RIOTBOARD)/common/esp32s2
endif
ifneq (,$(filter boards_common_esp32s3,$(USEMODULE)))
  DIRS += $(RIOTBOARD)/common/esp32s3
endif
ifneq (,$(filter boards_common_esp32x,$(USEMODULE)))
  DIRS += $(RIOTBOARD)/common/esp32x
endif
ifneq (,$(filter boards_common_seeedstudio-xiao-nrf52840,$(USEMODULE)))
  DIRS += $(RIOTBOARD)/common/seeedstudio-xiao-nrf52840
endif

include $(RIOTBASE)/Makefile.base
