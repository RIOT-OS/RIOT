APPLICATION = driver_dht
include ../Makefile.tests_common

FEATURES_REQUIRED = periph_gpio

# define default pin mappings for some boards:
ifneq (,$(filter stm32f4discovery,$(BOARD)))
  export DHT_GPIO ?= GPIO\(4,3\)
endif

USEMODULE += dht

ifneq (,$(DHT_TYPE))
  CFLAGS += -DDHT_TYPE=$(DHT_TYPE)
else
  # set random default
  CFLAGS += -DDHT_TYPE=DHT11
endif

ifneq (,$(DHT_GPIO))
  CFLAGS += -DDHT_GPIO=$(DHT_GPIO)
else
  # set random default
  CFLAGS += -DDHT_GPIO=GPIO\(0,0\)
endif

include $(RIOTBASE)/Makefile.include
