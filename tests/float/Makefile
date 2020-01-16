include ../Makefile.tests_common

# for native we can go do a couple of more operations in reasonable time...
ifneq (,$(filter native,$(BOARD)))
  CFLAGS += -DTEST_ITER=100000000
endif

include $(RIOTBASE)/Makefile.include
