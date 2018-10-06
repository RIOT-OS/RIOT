DISABLE_MODULE += auto_init

# for native we can go do a couple of more operations in reasonable time...
ifneq (,$(filter native,$(BOARD)))
  CFLAGS += -DTEST_ITER=100000000
endif

TEST_ON_CI_WHITELIST += all

include ../Makefile.tests_common
include $(RIOTBASE)/Makefile.include
