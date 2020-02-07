include ../Makefile.tests_common

BOARD_WHITELIST := sensebox_samd21 samr21-xpro nucleo-l152re nucleo-l432kc nucleo-l073rz b-l072z-lrwan arduino-nano

USEMODULE += ds18
USEMODULE += xtimer
USEMODULE += printf_float
# Use the module if you have an accurate sleep with xtimer (~3us)
#USEMODULE += ds18_optimized

include $(RIOTBASE)/Makefile.include
