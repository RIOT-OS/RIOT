include ../Makefile.core_common

USEMODULE += core_thread_flags
USEMODULE += core_thread_flags_group

ifneq (,$(filter native32 native64,$(BOARD)))
	# test non-trivial membership array on native
  CFLAGS += -DMAXTHREADS=64
  CFLAGS += -DWAITER_THREADS_CNT=40
endif

include $(RIOTBASE)/Makefile.include
