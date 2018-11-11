include ../Makefile.tests_common

FEATURES_REQUIRED = periph_gpio
FEATURES_OPTIONAL = periph_gpio_irq

USEMODULE += shell
USEMODULE += benchmark

bench:
	tests/02-bench.py

BOARD_INSUFFICIENT_MEMORY := \
  arduino-duemilanove \
  arduino-uno \
  #

include $(RIOTBASE)/Makefile.include
