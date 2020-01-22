include ../Makefile.tests_common

USEMODULE += ssp

include $(RIOTBASE)/Makefile.include

ifneq (,$(shell $(CC) --help=warnings | grep '\-Wstringop-overflow='))
  CFLAGS += -Wstringop-overflow=0
endif
