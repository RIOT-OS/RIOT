include ../Makefile.tests_common

DRIVER ?= sht11
BOARD ?= msba2

USEMODULE += $(DRIVER)
USEMODULE += shell
USEMODULE += saul_default
USEMODULE += shell_commands
USEMODULE += ps

BOARD_INSUFFICIENT_MEMORY := \
  arduino-duemilanove \
  arduino-uno \
  #

include $(RIOTBASE)/Makefile.include
