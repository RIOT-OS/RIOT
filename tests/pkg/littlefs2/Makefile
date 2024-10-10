include ../Makefile.pkg_common

USEPKG += littlefs2
USEMODULE += embunit
USEMODULE += mtd_emulated

# silence expected errors
CFLAGS += -DLOG_LEVEL=LOG_NONE

include $(RIOTBASE)/Makefile.include
