include ../Makefile.tests_common

USEMODULE += ssp

# This test intentionally crashes the firmware and when using a board with
# stdio via CDC ACM, RIOT can't print anything after the crash.
FEATURES_BLACKLIST += highlevel_stdio

include $(RIOTBASE)/Makefile.include

ifneq (,$(shell $(CC) --help=warnings | grep '\-Wstringop-overflow='))
  CFLAGS += -Wstringop-overflow=0
endif

ifneq (,$(shell $(CC) --help=warnings | grep '\-Warray-bounds='))
  CFLAGS += -Warray-bounds=0
endif
