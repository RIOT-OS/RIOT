DEVELHELP ?= 1

include ../Makefile.tests_common

# The bug is not fixed yet, so blacklist this test.
TEST_ON_CI_BLACKLIST += all
TEST_ON_CI_WHITELIST += native samr21-xpro

USEMODULE += tlsf-malloc

CFLAGS += -DDEBUG_ASSERT_VERBOSE -DTLSF_NATIVE_HEAPSIZE=0x4000

include $(RIOTBASE)/Makefile.include
