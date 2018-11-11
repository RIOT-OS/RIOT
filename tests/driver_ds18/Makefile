include ../Makefile.tests_common

USEMODULE += ds18
USEMODULE += xtimer
USEMODULE += printf_float
# Use the module if you have an accurate sleep with xtimer (~3us)
#USEMODULE += ds18_optimized

BOARD_WHITELIST := \
  nucleo-l152re \
  samr21-xpro \
  sensebox_samd21 \
  #

include $(RIOTBASE)/Makefile.include
