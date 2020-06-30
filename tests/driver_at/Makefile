include ../Makefile.tests_common

USEMODULE += shell
USEMODULE += at
USEMODULE += at_urc_isr_medium

# we are printing from the event thread, we need more stack
CFLAGS += -DEVENT_THREAD_MEDIUM_STACKSIZE=1024

include $(RIOTBASE)/Makefile.include
