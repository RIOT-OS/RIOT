include ../Makefile.tests_common

# nucleo-f303k8 doesn't have enough RAM to run the test so we reduce the stack
# size for every thread
ifneq (,$(filter nucleo-f303k8 nucleo-f334r8,$(BOARD)))
  CFLAGS += -DTHREAD_STACKSIZE_DEFAULT=512
endif

USEMODULE += cpp11-compat
USEMODULE += xtimer
USEMODULE += timex

include $(RIOTBASE)/Makefile.include
