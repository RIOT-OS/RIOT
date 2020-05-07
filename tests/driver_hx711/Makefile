include ../Makefile.tests_common

BOARD ?= bluepill
FEATURES_REQUIRED += periph_gpio
USEMODULE += hx711
CFLAGS += "-DHX711_PARAM_SCK=GPIO_PIN(1, 13)"
CFLAGS += "-DHX711_PARAM_DOUT=GPIO_PIN(1, 14)"

include $(RIOTBASE)/Makefile.include