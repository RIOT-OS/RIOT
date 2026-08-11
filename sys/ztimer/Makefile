# make all code end up in "ztimer_core.a"
MODULE := ztimer_core

MODULE_DEFS_NEEDED += \
  core_mbox \
  core_msg \
  core_thread_flags \
  periph_timer_query_freqs \
  pm_layered \
  stdio_rtt \
  stdio_semihosting \
  ztimer_auto_adjust \
  ztimer_extend \
  ztimer_msec \
  ztimer_ondemand \
  ztimer_ondemand_rtc \
  ztimer_ondemand_rtt \
  ztimer_ondemand_strict \
  ztimer_ondemand_timer \
  ztimer_periph_lptimer \
  ztimer_periph_rtc \
  ztimer_periph_rtt \
  ztimer_periph_timer \
  ztimer_sec \
  ztimer_usec \
  #

# ensure that "ztimer_foo" builds "ztimer_foo.c", not "ztimer_core_foo.c"
BASE_MODULE := ztimer

# ztimer_core files
SRC := core.c util.c periodic.c

# enable submodules
SUBMODULES := 1

include $(RIOTBASE)/Makefile.base
