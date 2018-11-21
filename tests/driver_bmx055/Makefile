include ../Makefile.tests_common

BOARD_INSUFFICIENT_MEMORY := arduino-duemilanove arduino-uno

# include and auto-initialize all available sensors
USEMODULE += saul_default
# include driver for bmx055 sensor
USEMODULE += bmx055
# include xtimer for polling
USEMODULE += xtimer

include $(RIOTBASE)/Makefile.include
