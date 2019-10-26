include ../Makefile.tests_common

USEMODULE += ds18
USEMODULE += xtimer
USEMODULE += printf_float
# Use the module if you have an accurate sleep with xtimer (~3us)
#USEMODULE += ds18_optimized

BOARD_WHITELIST := \
  b-l072z-lrwan \
  nucleo-l073rz \
  nucleo-l152re \
  nucleo-l432kc \
  samr21-xpro \
  sensebox_samd21 \
  #

include $(RIOTBASE)/Makefile.include
