export APPLICATION = periph_gpio
include ../Makefile.tests_common

FEATURES_REQUIRED = periph_gpio

ifneq (,$(filter native,$(BOARD)))
  USEMODULE += native_gpio_dummy
endif

DISABLE_MODULE += auto_init

include $(RIOTBASE)/Makefile.include
