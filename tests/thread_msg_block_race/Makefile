DEVELHELP := 1

include ../Makefile.tests_common

BOARD_INSUFFICIENT_MEMORY := arduino-duemilanove arduino-nano arduino-uno \
                             nucleo-f031k6

DISABLE_MODULE += auto_init
FEATURES_REQUIRED += periph_timer
USEMODULE += random

include $(RIOTBASE)/Makefile.include
