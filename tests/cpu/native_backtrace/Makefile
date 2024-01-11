include ../Makefile.cpu_common

USEMODULE += backtrace

BOARD_WHITELIST := native

include $(RIOTBASE)/Makefile.include

ifeq ($(BOARD),native)
  ifneq (,$(filter arch_64bit,$(FEATURES_USED)))
    # 64 bit
    CFLAGS += -DBACKTRACE_SIZE=3
  else
    # 32 bit
    CFLAGS += -DBACKTRACE_SIZE=4
  endif
endif
