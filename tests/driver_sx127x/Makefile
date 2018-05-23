include ../Makefile.tests_common

BOARD_INSUFFICIENT_MEMORY := nucleo-f031k6

BOARD ?= nucleo-l152re

USEMODULE += od
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps
USEMODULE += xtimer

DRIVER ?= sx1276

# use SX1276 by default
USEMODULE += $(DRIVER)

FEATURES_REQUIRED ?= periph_spi
FEATURES_REQUIRED ?= periph_gpio

include $(RIOTBASE)/Makefile.include
