include ../Makefile.tests_common

BOARD_INSUFFICIENT_MEMORY := arduino-duemilanove arduino-nano \
                             arduino-uno nucleo-f031k6

CFLAGS += -DTASKLET_HANDLER_PRIO=THREAD_PRIORITY_MAIN+1

USEMODULE += tasklet
USEMODULE += xtimer

include $(RIOTBASE)/Makefile.include
