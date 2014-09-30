export APPLICATION = periph_gpio
include ../Makefile.tests_common

FEATURES_REQUIRED = periph_gpio

ifneq (,$(filter native,$(BOARD)))
  USEMODULE += native_gpio_dummy
  # comment out above and uncomment below to use the sysfs interface instead
  #CFLAGS += -DNATIVE_GPIO_NUMOF=4
  #USEMODULE += native_gpio_sysfs
endif

DISABLE_MODULE += auto_init

include $(RIOTBASE)/Makefile.include
