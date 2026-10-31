include ../Makefile.cpu_common

USEMODULE += ztimer_usec
USEMODULE += ztimer_msec

BOARDS_SUPPORTED := native32 native64

# the test uses SSE registers, which are not enabled by default for i386
ifeq (native32,$(BOARD))
  CFLAGS += -msse2
endif

include $(RIOTBASE)/Makefile.include
