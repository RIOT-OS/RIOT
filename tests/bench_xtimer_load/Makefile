include ../Makefile.tests_common

USEMODULE += xtimer

# configure benchmark frequency
TEST_HZ ?= 64
CFLAGS += -DTEST_HZ=$(TEST_HZ)LU

# microbit qemu failing currently
TEST_ON_CI_BLACKLIST += microbit

include $(RIOTBASE)/Makefile.include
