include ../Makefile.tests_common

BOARD_INSUFFICIENT_MEMORY := arduino-duemilanove arduino-nano \
                             arduino-uno nucleo-f031k6

CFLAGS += -DIRQ_HANDLER_PRIO=THREAD_PRIORITY_MAIN+1

USEMODULE += irq_handler
USEMODULE += xtimer

include $(RIOTBASE)/Makefile.include
