include ../Makefile.tests_common

USEMODULE += trace

# reduce tracebuffer (default is 512), so this test compiles for more boards
CFLAGS += -DCONFIG_TRACE_BUFSIZE=64

include $(RIOTBASE)/Makefile.include
