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

# Fails on esp32 because the driver defines default GPIOs that are used for the
# SPI flash interface.
TEST_ON_CI_BLACKLIST += esp32-wroom-32

include $(RIOTBASE)/Makefile.include
