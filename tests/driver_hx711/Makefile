include ../Makefile.tests_common

USEMODULE += hx711
USEMODULE += xtimer

# Change these values, if your device is connected on different pins
CFLAGS += "-DHX711_PARAM_SCK=GPIO_PIN(1, 13)"
CFLAGS += "-DHX711_PARAM_DOUT=GPIO_PIN(1, 14)"

include $(RIOTBASE)/Makefile.include
