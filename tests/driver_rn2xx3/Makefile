include ../Makefile.tests_common

DRIVER ?= rn2483

USEMODULE += $(DRIVER)
USEMODULE += shell
USEMODULE += shell_commands

BOARD_INSUFFICIENT_MEMORY := \
  arduino-duemilanove \
  arduino-uno \
  #

include $(RIOTBASE)/Makefile.include
