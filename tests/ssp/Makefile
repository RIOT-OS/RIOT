include ../Makefile.tests_common

USEMODULE += ssp

# This test intentionally crashes the firmware and when using a board with a
# preflashed bootloader, it cannot be reflashed automatically afterwards.
FEATURES_BLACKLIST += bootloader_arduino
FEATURES_BLACKLIST += bootloader_nrfutil

include $(RIOTBASE)/Makefile.include

ifneq (,$(shell $(CC) --help=warnings | grep '\-Wstringop-overflow='))
  CFLAGS += -Wstringop-overflow=0
endif
