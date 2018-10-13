include ../Makefile.tests_common

BOARD := nucleo-f411re

USEMODULE += xtimer
USEMODULE += il3829

CFLAGS += -DDISPLAY_X=200
CFLAGS += -DDISPLAY_Y=200
CFLAGS += -DIL3829_PARAM_CS=GPIO_PIN\(PORT_B,6\)
CFLAGS += -DIL3829_PARAM_DC=GPIO_PIN\(PORT_C,7\)

include $(RIOTBASE)/Makefile.include
