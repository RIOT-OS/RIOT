BOARD ?= b-l072z-lrwan1
include ../Makefile.tests_common

FEATURES_REQUIRED += periph_eeprom

USEMODULE += shell
USEMODULE += shell_commands  # provides reboot command

BOARD_INSUFFICIENT_MEMORY := \
  arduino-duemilanove \
  arduino-uno \
  #

include $(RIOTBASE)/Makefile.include
