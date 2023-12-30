BOARD ?= nucleo-f303k8
include ../Makefile.drivers_common

# Update this to your needs
# N ?= 8

USEMODULE += ws281x_spi
USEMODULE += xtimer

EXTERNAL_BOARD_DIRS += $(RIOTBASE)/tests/build_system/external_board_dirs/esp-ci-boards

include $(RIOTBASE)/Makefile.include

ifneq (, $(N))
  CFLAGS += '-DWS281X_PARAM_NUMOF=$(N)'
endif
