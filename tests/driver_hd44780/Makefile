include ../Makefile.tests_common

USEMODULE += hd44780

# disable native GPIOs for automatic test
ifeq (native,$(BOARD))
  USEMODULE += periph_gpio_mock
  # the same for Kconfig
  ifeq (1,$(TEST_KCONFIG))
    KCONFIG_ADD_CONFIG += $(APPDIR)/app.config.test.native
  endif
endif

include $(RIOTBASE)/Makefile.include
