include ../Makefile.drivers_common

USEMODULE += hd44780

# disable native GPIOs for automatic test
ifneq (,$(filter native native32,$(BOARD)))
  USEMODULE += periph_gpio_mock
endif

include $(RIOTBASE)/Makefile.include
