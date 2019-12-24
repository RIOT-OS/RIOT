include ../Makefile.tests_common

DISABLE_MODULE += auto_init

# stm32f030f4-demo doesn't have enough RAM to run the test
# so we reduce the stack size for every thread
ifneq (,$(filter stm32f030f4-demo,$(BOARD)))
  CFLAGS += -DTHREAD_STACKSIZE_DEFAULT=512
endif

include $(RIOTBASE)/Makefile.include
