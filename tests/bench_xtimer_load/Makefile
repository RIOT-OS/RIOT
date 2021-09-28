include ../Makefile.tests_common

USEMODULE += ztimer_usec

# configure benchmark frequency
TEST_HZ ?= 64
CFLAGS += -DTEST_HZ=$(TEST_HZ)LU

include $(RIOTBASE)/Makefile.include
