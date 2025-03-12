include ../Makefile.cpu_common

USEMODULE += backtrace

BOARD_WHITELIST := native32 native64

# Tests if native returns a backtrace of size three.
# The following function should be included in the backtrace:
# `main`/`main_trampoline`/ return to a user context function (e.g. `makecontext`)
# Depending on the implementation of the ucontext functions, the backtrace size
# may be longer, but this test only checks if it is at least three.
CFLAGS += -DBACKTRACE_SIZE=3

include $(RIOTBASE)/Makefile.include
