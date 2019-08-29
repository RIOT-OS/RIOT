include ../Makefile.tests_common

BUSY_PIN ?= GPIO_UNDEF
BOARD_INSUFFICIENT_MEMORY := arduino-duemilanove arduino-leonardo arduino-nano\
                             arduino-uno

USEMODULE += dfplayer
USEMODULE += shell
USEMODULE += shell_commands

include $(RIOTBASE)/Makefile.include

CFLAGS += '-DDFPLAYER_PARAM_BUSY_PIN=$(BUSY_PIN)'
