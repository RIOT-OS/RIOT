include ../Makefile.bench_common

USEMODULE += xtimer

# configure benchmark frequency
TEST_HZ ?= 64
CFLAGS += -DTEST_HZ=$(TEST_HZ)LU

# microbit qemu failing currently
TEST_ON_CI_BLACKLIST += microbit

# This test randomly fails on `native` so disable it from CI
TEST_ON_CI_BLACKLIST += native32 native64

include $(RIOTBASE)/Makefile.include
