BOARD ?= atmega328p
include ../Makefile.drivers_common

# Update this to your needs
# PIN ?= GPIO_PIN(0, 0)
# N ?= 8

# For SPI implementation only
# DEV ?= 0
# DMA ?= 0
# LEN ?= 3
# CLK ?= 2400000

USEMODULE += ws281x
USEMODULE += xtimer

# Only AVR boards CPU clocked at 8MHz or 16 MHz are supported. The Waspmote Pro
# is clocked at 14.7456 MHz :-/
BOARD_BLACKLIST := waspmote-pro

EXTERNAL_BOARD_DIRS += $(RIOTBASE)/tests/build_system/external_board_dirs/esp-ci-boards

ifneq (, $(PIN))
  CFLAGS += '-DWS281X_PARAM_PIN=$(PIN)'
endif
ifneq (, $(N))
  CFLAGS += '-DWS281X_PARAM_NUMOF=$(N)'
endif

ifneq (, $(DEV))
  CFLAGS += '-DWS281X_SPI_DEV=$(DEV)'
endif
ifeq (1, $(DMA))
  FEATURES_REQUIRED += periph_dma
endif
ifneq (, $(LEN))
  CFLAGS += '-DWS281X_SPI_PATTERN_LENGTH=$(LEN)'
endif
ifneq (, $(CLK))
  CFLAGS += '-DWS281X_SPI_CLK=$(CLK)'
endif

include $(RIOTBASE)/Makefile.include
