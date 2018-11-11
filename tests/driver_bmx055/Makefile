include ../Makefile.tests_common

# include and auto-initialize all available sensors
USEMODULE += saul_default
# include driver for bmx055 sensor
USEMODULE += bmx055
# include xtimer for polling
USEMODULE += xtimer

BOARD_INSUFFICIENT_MEMORY := \
  arduino-duemilanove \
  arduino-uno \
  #

include $(RIOTBASE)/Makefile.include
