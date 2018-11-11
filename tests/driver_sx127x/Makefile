include ../Makefile.tests_common

BOARD ?= nucleo-l152re

USEMODULE += od
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps
USEMODULE += xtimer

DRIVER ?= sx1276

# use SX1276 by default
USEMODULE += $(DRIVER)

BOARD_INSUFFICIENT_MEMORY := \
  arduino-duemilanove \
  arduino-uno \
  nucleo-f031k6 \
  #

include $(RIOTBASE)/Makefile.include
