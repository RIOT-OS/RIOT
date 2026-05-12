BOARD ?= b-l072z-lrwan1

# Build with: make PING=1 for transmit mode
# Default: receive mode

ifdef PING
	CFLAGS += -DPING
endif

include ../Makefile.pkg_common

USEPKG += radiolib

FEATURES_REQUIRED += periph_gpio periph_gpio_irq periph_spi
USEMODULE += ztimer_msec ztimer_usec

include $(RIOTBASE)/Makefile.include
