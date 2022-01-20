BOARD ?= stm32f429i-disc1
include ../Makefile.tests_common

# No interactive_sync
DISABLE_MODULE += test_utils_interactive_sync

USEPKG += lvgl
USEMODULE += lvgl_contrib

# SDL requires more stack
ifeq (native,$(BOARD))
  CFLAGS += -DTHREAD_STACKSIZE_MAIN=64*1024
else
  CFLAGS += -DTHREAD_STACKSIZE_MAIN=2048
endif

include $(RIOTBASE)/Makefile.include
