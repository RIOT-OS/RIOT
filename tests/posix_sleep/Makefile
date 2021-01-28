include ../Makefile.tests_common

USEMODULE += posix_sleep

# These CPU families have a non configurable RTT of 1Hz, not enough for
# ztimer_msec to run on periph_rtt
ifeq (,$(filter efm32 kinetis,$(CPU)))
  # Pull-in periph-rtt on boards that provide this feature to switch to
  # the RTT backend of ztimer
  FEATURES_OPTIONAL += periph_rtt
endif

include $(RIOTBASE)/Makefile.include
