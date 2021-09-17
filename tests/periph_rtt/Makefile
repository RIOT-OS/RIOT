BOARD ?= samr21-xpro
include ../Makefile.tests_common

FEATURES_REQUIRED = periph_rtt
FEATURES_OPTIONAL += periph_rtt_set_counter
FEATURES_OPTIONAL += periph_rtc_mem

DISABLE_MODULE += periph_init_rtt

# avoid running Kconfig by default
SHOULD_RUN_KCONFIG ?=

include $(RIOTBASE)/Makefile.include

# Put board specific dependencies here
ifneq (,$(filter stm32 kinetis,$(CPU)))
  ifneq (f1,$(CPU_FAM))
    # all stm32% but stm32f1 RTT are based on a 16 bit LPTIM, if using the default
    # 32768KHz configuration TICKS_TO_WAIT will overflow
    RTT_FREQUENCY ?= 1024
    CFLAGS += -DRTT_FREQUENCY=$(RTT_FREQUENCY)
  endif
endif
