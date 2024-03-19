include ../Makefile.core_common

USEMODULE += fmt
USEMODULE += core_mutex_priority_inheritance

ifneq ($(RIOT_CI_BUILD),1)
  # For human beings add a busy delay to the mid priority task to make the problem more approachable
  FANCY ?= 1
else
  # Skip the fancy delay for the CI to not waste precious CI time
  FANCY ?= 0
endif

include $(RIOTBASE)/Makefile.include

CFLAGS += -DFANCY=$(FANCY)
CFLAGS += -DTHREAD_STACKSIZE_MAIN=THREAD_STACKSIZE_SMALL
