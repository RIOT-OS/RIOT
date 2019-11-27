DEVELHELP ?= 0
include ../Makefile.tests_common

CFLAGS += -DNDEBUG -DLOG_LEVEL=LOG_NONE

DISABLE_MODULE += auto_init
DISABLE_MODULE += test_utils_interactive_sync

include $(RIOTBASE)/Makefile.include
