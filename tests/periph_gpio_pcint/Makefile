include ../Makefile.tests_common

BOARD ?= arduino-uno

FEATURES_REQUIRED += periph_gpio
FEATURES_REQUIRED += periph_gpio_irq
# At least one PCINT port is required
FEATURES_REQUIRED += atmega_pcint0

USEMODULE += atmega_pcint

include $(RIOTBASE)/Makefile.include

ifeq (arduino-uno,$(BOARD))
  CFLAGS += "-DTHREAD_STACKSIZE_DEFAULT=256"
endif
