include ../Makefile.drivers_common

USEMODULE += hd44780

# disable native GPIOs for automatic test
ifneq (,$(filter native native64,$(BOARD)))
  USEMODULE += periph_gpio_mock
  # the same for Kconfig
  ifeq (1,$(TEST_KCONFIG))
    KCONFIG_ADD_CONFIG += $(APPDIR)/app.config.test.native
  endif
endif

include $(RIOTBASE)/Makefile.include
