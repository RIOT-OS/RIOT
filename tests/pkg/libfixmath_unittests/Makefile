GOOD_REASON_FOR_USING_FLOATS := "FPU used to compare fixmath operations against"

include ../Makefile.pkg_common

USEMODULE += libfixmath-unittests

ifneq (,$(filter native,$(BOARD)))
  LINKFLAGS += -lm
endif

USEMODULE += printf_float
USEMODULE += test_utils_main_exit_cb

CFLAGS += -Wno-error=strict-prototypes
CFLAGS += -Wno-error=old-style-definition
CFLAGS += -Wno-error=format

TEST_ON_CI_WHITELIST += native

include $(RIOTBASE)/Makefile.include
