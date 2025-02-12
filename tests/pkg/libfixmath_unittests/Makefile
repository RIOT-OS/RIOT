include ../Makefile.pkg_common

USEMODULE += libfixmath-unittests

ifneq (,$(filter native native32,$(BOARD)))
  LINKFLAGS += -lm
endif

USEMODULE += printf_float
USEMODULE += test_utils_main_exit_cb

CFLAGS += -Wno-error=strict-prototypes
CFLAGS += -Wno-error=old-style-definition
CFLAGS += -Wno-error=format

TEST_ON_CI_WHITELIST += native native32

include $(RIOTBASE)/Makefile.include
