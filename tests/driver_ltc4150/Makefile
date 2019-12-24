include ../Makefile.tests_common

BOARD ?= msba2

USEMODULE += fmt_table
USEMODULE += ltc4150

include $(RIOTBASE)/Makefile.include

ifneq (,$(filter $(BOARD),msb-430 msb-430h telosb wsn430-v1_3b wsn430-v1_4 z1))
  CFLAGS += -DNO_FPUTS
endif
