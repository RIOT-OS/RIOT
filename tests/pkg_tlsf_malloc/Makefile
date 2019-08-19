DEVELHELP ?= 1

include ../Makefile.tests_common

# The bug is not fixed yet, so blacklist this test.
TEST_ON_CI_BLACKLIST += all

USEMODULE += tlsf-malloc

CFLAGS += -DDEBUG_ASSERT_VERBOSE -DTLSF_NATIVE_HEAPSIZE=0x4000

include $(RIOTBASE)/Makefile.include
