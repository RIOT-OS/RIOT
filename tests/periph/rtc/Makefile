include ../Makefile.periph_common

FEATURES_REQUIRED += periph_rtc
FEATURES_OPTIONAL += periph_rtc_ms periph_rtc_mem

DISABLE_MODULE += periph_init_rtc

USEMODULE += shell ztimer ztimer_msec

# avoid running Kconfig by default
SHOULD_RUN_KCONFIG ?=

include $(RIOTBASE)/Makefile.include
