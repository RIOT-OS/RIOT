DEVELHELP ?= 0
include ../Makefile.tests_common

CFLAGS += -DNDEBUG -DLOG_LEVEL=LOG_NONE

DISABLE_MODULE += auto_init auto_init_%
DISABLE_MODULE += test_utils_interactive_sync
DISABLE_MODULE += test_utils_print_stack_usage

USEMODULE += stdio_null

# adjust stack sizes to very small values
ifeq (,$(filter native32 native64,$(BOARD)))
  CFLAGS += -DTHREAD_STACKSIZE_MAIN=256 -DTHREAD_STACKSIZE_IDLE=128
endif

include $(RIOTBASE)/Makefile.include
