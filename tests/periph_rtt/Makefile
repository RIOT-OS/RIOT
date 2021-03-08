BOARD ?= samr21-xpro
include ../Makefile.tests_common

FEATURES_REQUIRED = periph_rtt
FEATURES_OPTIONAL += periph_rtt_set_counter

DISABLE_MODULE += periph_init_rtt

include $(RIOTBASE)/Makefile.include

# Put board specific dependencies here
ifeq (stm32,$(CPU))
  ifneq (f1,$(CPU_FAM))
    # all stm32% but stm32f1 RTT are based on a 16 bit LPTIM, if using the default
    # 32768KHz configuration TICKS_TO_WAIT will overflow
    RTT_FREQUENCY ?= 1024
    CFLAGS += -DRTT_FREQUENCY=$(RTT_FREQUENCY)
  endif
endif
