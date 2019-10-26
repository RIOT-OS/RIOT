include ../Makefile.tests_common

USEPKG += lua

# This application uses getchar and thus expects input from stdio
USEMODULE += stdin

ifneq ($(BOARD),native)
  # This stack size is large enough to run Lua print() functions of
  # various lengths. Other functions untested.
  CFLAGS += -DTHREAD_STACKSIZE_MAIN='(THREAD_STACKSIZE_DEFAULT+2048)'
endif

BOARD_WHITELIST += \
  native \
  samr21-xpro \
  #

include $(RIOTBASE)/Makefile.include
