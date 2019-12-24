include ../Makefile.tests_common

FORCE_ASSERTS = 1
USEMODULE += event_callback
USEMODULE += event_timeout

# stm32f030f4-demo doesn't have enough RAM to run the test
# so we reduce the stack size for every thread
ifneq (,$(filter stm32f030f4-demo,$(BOARD)))
  CFLAGS += -DTHREAD_STACKSIZE_DEFAULT=640
endif

include $(RIOTBASE)/Makefile.include
