include ../Makefile.tests_common

BOARD ?= msba2

USEMODULE += fmt_table
USEMODULE += ltc4150
USEMODULE += ztimer_usec

include $(RIOTBASE)/Makefile.include

ifneq (,$(filter $(BOARD),msb-430 msb-430h telosb z1))
  CFLAGS += -DNO_FPUTS
endif
