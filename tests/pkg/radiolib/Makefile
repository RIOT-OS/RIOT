BOARD ?= b-l072z-lrwan1

# Build with: make PING=1 for transmit mode
# Default: receive mode

ifdef PING
	CFLAGS += -DPING
endif

include ../Makefile.pkg_common

RADIOLIB_DRIVER ?= radiolib_sx127x

USEPKG += radiolib
USEMODULE += $(RADIOLIB_DRIVER)
USEMODULE += auto_init_radiolib

FEATURES_REQUIRED += periph_gpio periph_gpio_irq periph_spi
USEMODULE += ztimer_msec ztimer_usec printf_float

include $(RIOTBASE)/Makefile.include
