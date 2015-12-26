APPLICATION = driver_dht
include ../Makefile.tests_common

FEATURES_REQUIRED = periph_gpio

USEMODULE += dht
USEMODULE += xtimer

# define parameters for selected boards
ifneq (,$(filter stm32f4discovery,$(BOARD)))
  DHT_GPIO ?= GPIO_PIN\(4,3\)
endif

# set default device parameters in case they are undefined
DHT_GPIO ?= GPIO_PIN\(0,0\)
DHT_TYPE ?= DHT11

# export parameters
CFLAGS += -DDHT_TYPE=$(DHT_TYPE)
CFLAGS += -DDHT_GPIO=$(DHT_GPIO)

include $(RIOTBASE)/Makefile.include
