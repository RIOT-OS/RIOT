include ../Makefile.sys_common

# required modules
USEMODULE += ztimer
USEMODULE += ztimer_ondemand
USEMODULE += ztimer_ondemand_timer
USEMODULE += ztimer_ondemand_rtt
USEMODULE += ztimer_ondemand_rtc
FEATURES_REQUIRED = periph_timer
DEFAULT_MODULE += test_utils_interactive_sync

# Select clock to benchmark:
# - ztimer_usec
USEMODULE += ztimer_usec
# - ztimer_msec
#USEMODULE += ztimer_msec
# - ztimer_sec
#USEMODULE += ztimer_sec

# You may sepcify the compare timer device
#CFLAGS += '-DCOMPARE_TIMER_DEV=TIMER_DEV(2)'

# You may sepcify the compare timer frequency
#CFLAGS += '-DCOMPARE_TIMER_FREQ=9750000'

include $(RIOTBASE)/Makefile.include
