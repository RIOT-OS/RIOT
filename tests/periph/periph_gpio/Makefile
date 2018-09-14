include ../Makefile.tests_common

BOARD_INSUFFICIENT_MEMORY := arduino-duemilanove arduino-uno

FEATURES_REQUIRED = periph_gpio
FEATURES_REQUIRED = periph_gpio_irq

USEMODULE += shell
USEMODULE += benchmark

include $(TESTBASE)/../Makefile.include

bench:
	tests/02-bench.py
