include ../Makefile.tests_common

CFLAGS += -DIRQ_HANDLER_PRIO=THREAD_PRIORITY_MAIN+1

USEMODULE += irq_handler
USEMODULE += xtimer

include $(RIOTBASE)/Makefile.include
