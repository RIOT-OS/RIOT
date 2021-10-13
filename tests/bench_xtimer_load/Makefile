include ../Makefile.tests_common

USEMODULE += xtimer

# configure benchmark frequency
TEST_HZ ?= 64
CFLAGS += -DTEST_HZ=$(TEST_HZ)LU

include $(RIOTBASE)/Makefile.include
