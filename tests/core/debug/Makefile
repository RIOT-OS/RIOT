include ../Makefile.core_common

USEMODULE += ztimer_sec

# make sure ISR stack is big enough to handle debug printf
CFLAGS += -DISR_STACKSIZE="(THREAD_STACKSIZE_MINIMUM+THREAD_EXTRA_STACKSIZE_PRINTF)"

include $(RIOTBASE)/Makefile.include
