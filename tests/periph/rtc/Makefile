include ../Makefile.periph_common

FEATURES_REQUIRED += periph_rtc
FEATURES_OPTIONAL += periph_rtc_ms
FEATURES_OPTIONAL += periph_rtc_mem

USEMODULE += periph_rtc_setter_callback

DISABLE_MODULE += periph_init_rtc

USEMODULE += xtimer

# avoid running Kconfig by default
SHOULD_RUN_KCONFIG ?=

include $(RIOTBASE)/Makefile.include
