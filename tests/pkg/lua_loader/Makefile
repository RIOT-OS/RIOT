include ../Makefile.pkg_common

USEPKG += lua

# This application uses getchar and thus expects input from stdio
USEMODULE += stdin

BOARD_WHITELIST += native native64 samr21-xpro

ifeq (,$(filter native native64,$(BOARD)))
  # This stack size is large enough to run Lua print() functions of
  # various lengths. Other functions untested.
  CFLAGS += -DTHREAD_STACKSIZE_MAIN='(THREAD_STACKSIZE_DEFAULT+2048)'
endif

include $(RIOTBASE)/Makefile.include
