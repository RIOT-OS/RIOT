include ../Makefile.core_common

# disable early exit optimization to facilitate the testing of some edge cases
CFLAGS += -DCONFIG_QUEUE_WAIT_EARLY_EXIT=0

USEMODULE += ztimer
USEMODULE += ztimer_msec
USEMODULE += sema

DEVELHELP = 1
VERBOSE_ASSERT = 1
CFLAGS_OPT += -Og -g

include $(RIOTBASE)/Makefile.include
