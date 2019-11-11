include ../Makefile.tests_common

USEMODULE += libfixmath-unittests

ifneq (,$(filter native,$(BOARD)))
  export LINKFLAGS += -lm
endif

USEMODULE += printf_float

TEST_ON_CI_WHITELIST += native

include $(RIOTBASE)/Makefile.include
