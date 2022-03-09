BOARD ?= samr34-xpro

include ../Makefile.tests_common

FEATURES_REQUIRED += backup_ram
FEATURES_REQUIRED += periph_rtc

USEMODULE += pm_layered
USEMODULE += xtimer

DISABLE_MODULE += test_utils_interactive_sync

CFLAGS += -DLOG_LEVEL=LOG_WARNING

# avoid running Kconfig by default
SHOULD_RUN_KCONFIG ?=

include $(RIOTBASE)/Makefile.include
