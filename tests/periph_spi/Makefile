BOARD ?= samr21-xpro
include ../Makefile.tests_common

FEATURES_REQUIRED = periph_spi

USEMODULE += xtimer
USEMODULE += shell
USEMODULE += shell_commands

BOARD_INSUFFICIENT_MEMORY := \
  arduino-duemilanove \
  arduino-uno \
  #

include $(RIOTBASE)/Makefile.include
