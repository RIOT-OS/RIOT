include ../Makefile.tests_common

USEMODULE += xtimer

# This test randomly fails on `native` so disable it from CI
TEST_ON_CI_BLACKLIST += native

# stm32f030f4-demo doesn't have enough RAM to run the test
# so we reduce the stack size for every thread
ifneq (,$(filter stm32f030f4-demo,$(BOARD)))
  CFLAGS += -DTHREAD_STACKSIZE_DEFAULT=512
endif

include $(RIOTBASE)/Makefile.include
