DEVELHELP ?= 0
include ../Makefile.tests_common

CFLAGS += -DNDEBUG -DLOG_LEVEL=LOG_NONE

DISABLE_MODULE += auto_init auto_init_%
DISABLE_MODULE += test_utils_interactive_sync

USEMODULE += stdio_null

# adjust stack sizes to very small values
ifneq (native,$(BOARD))
  CFLAGS += -DTHREAD_STACKSIZE_MAIN=256 -DTHREAD_STACKSIZE_IDLE=128
endif

include $(RIOTBASE)/Makefile.include
