include ../Makefile.pkg_common

USEMODULE += littlefs
USEMODULE += embunit
USEMODULE += mtd_emulated

# silence expected errors
CFLAGS += -DLOG_LEVEL=LOG_NONE

include $(RIOTBASE)/Makefile.include
