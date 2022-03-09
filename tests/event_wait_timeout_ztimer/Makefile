include ../Makefile.tests_common

FORCE_ASSERTS = 1
USEMODULE += event
USEMODULE += ztimer_usec ztimer_msec
USEMODULE += core_thread_flags

# stm32f030f4-demo doesn't have enough RAM to run the test
# so we reduce the stack size for every thread
ifneq (,$(filter stm32f030f4-demo,$(BOARD)))
  CFLAGS += -DTHREAD_STACKSIZE_DEFAULT=512
endif


include $(RIOTBASE)/Makefile.include
