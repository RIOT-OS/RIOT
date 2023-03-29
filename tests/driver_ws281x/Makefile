BOARD ?= atmega328p
include ../Makefile.tests_common

# Update this to your needs
# PIN ?= GPIO_PIN(0, 0)
# N ?= 8

USEMODULE += ws281x
USEMODULE += xtimer

# Only AVR boards CPU clocked at 8MHz or 16 MHz are supported. The Waspmote Pro
# is clocked at 14.7456 MHz :-/
BOARD_BLACKLIST := waspmote-pro

EXTERNAL_BOARD_DIRS += $(RIOTBASE)/tests/external_board_dirs/esp-ci-boards

include $(RIOTBASE)/Makefile.include

ifneq (, $(PIN))
  CFLAGS += '-DWS281X_PARAM_PIN=$(PIN)'
endif
ifneq (, $(N))
  CFLAGS += '-DWS281X_PARAM_NUMOF=$(N)'
endif
