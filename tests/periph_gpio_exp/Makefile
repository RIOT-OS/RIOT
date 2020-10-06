include ../Makefile.tests_common

FEATURES_OPTIONAL = periph_gpio_irq

USEMODULE += gpio_exp
USEMODULE += shell
USEMODULE += benchmark

INCLUDES += -I$(APPDIR)

include $(RIOTBASE)/Makefile.include

bench:
	tests/02-bench.py
