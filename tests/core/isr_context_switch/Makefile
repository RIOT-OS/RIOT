include ../Makefile.core_common

FEATURES_REQUIRED += periph_timer
FEATURES_OPTIONAL += periph_timer_query_freqs

USEMODULE += random

include $(RIOTBASE)/Makefile.include

CFLAGS += -DSCHED_TEST_STACK

# Optimize RAM usage rather than filling Makefile.ci:
CFLAGS += -DTHREAD_STACKSIZE_MAIN=THREAD_STACKSIZE_DEFAULT
CFLAGS += -DMAX_THREADS=4
