
MODULE_DEFS_NEEDED += \
  bme280_i2c \
  bme280_spi \
  #
include ../Makefile.drivers_common

DRIVER ?= bme280_i2c

USEMODULE += fmt
USEMODULE += xtimer
USEMODULE += $(DRIVER)

include $(RIOTBASE)/Makefile.include
