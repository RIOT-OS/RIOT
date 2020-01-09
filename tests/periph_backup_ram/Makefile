include ../Makefile.tests_common

FEATURES_REQUIRED = backup_ram

USEMODULE += pm_layered
USEMODULE += periph_rtc
USEMODULE += xtimer

DISABLE_MODULE += test_utils_interactive_sync

CFLAGS += -DLOG_LEVEL=LOG_WARNING

include $(RIOTBASE)/Makefile.include
