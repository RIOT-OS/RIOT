include ../Makefile.cpu_common

USEMODULE += backtrace

BOARDS_SUPPORTED := native32 native64

# Tests if native returns a backtrace of size three.
# The following function should be included in the backtrace:
# `main`/`main_trampoline`/ the host thread entry function (`_thread_entry`)
# Depending on the implementation of the host thread library, the backtrace size
# may be longer, but this test only checks if it is at least three.
CFLAGS += -DBACKTRACE_SIZE=3

include $(RIOTBASE)/Makefile.include
