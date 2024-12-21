include ../Makefile.core_common

USEMODULE += core_thread_flags
USEMODULE += xtimer

# microbit qemu timing is off
TEST_ON_CI_BLACKLIST += microbit

# This test randomly fails on `native` so disable it from CI
TEST_ON_CI_BLACKLIST += native native32

include $(RIOTBASE)/Makefile.include
