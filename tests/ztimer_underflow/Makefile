include ../Makefile.tests_common

TEST_ZTIMER_CLOCK ?= ZTIMER_USEC

USEMODULE += ztimer
USEMODULE += ztimer_usec
ifeq ($(TEST_ZTIMER_CLOCK), ZTIMER_MSEC)
  USEMODULE += ztimer_msec
	# the same for Kconfig
  ifeq (1,$(TEST_KCONFIG))
    KCONFIG_ADD_CONFIG += $(APPDIR)/app.config.msec.test
  endif
endif
ifeq ($(TEST_ZTIMER_CLOCK), ZTIMER_SEC)
  USEMODULE += ztimer_sec
endif

CFLAGS += -DTEST_ZTIMER_CLOCK=$(TEST_ZTIMER_CLOCK)

include $(RIOTBASE)/Makefile.include
