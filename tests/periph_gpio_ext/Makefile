include ../Makefile.tests_common

FEATURES_REQUIRED = periph_gpio
FEATURES_REQUIRED = periph_gpio_ext
FEATURES_OPTIONAL = periph_gpio_irq

USEMODULE += extend_gpio
USEMODULE += shell
USEMODULE += benchmark

INCLUDES += -I$(APPDIR)

include $(RIOTBASE)/Makefile.include

bench:
	tests/02-bench.py
