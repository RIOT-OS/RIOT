include ../Makefile.tests_common

# Set to 1 to disable the round-robin scheduling module
NORR ?= 0

ifneq (1,$(NORR))
  USEMODULE += sched_round_robin
endif

include $(RIOTBASE)/Makefile.include
