BOARD ?= b-l072z-lrwan1

include ../Makefile.pkg_common

USEPKG += radiolib

FEATURES_REQUIRED += periph_gpio periph_gpio_irq periph_spi
USEMODULE += ztimer_msec ztimer_usec

# Build with: make CXXEXTRAFLAGS="-DRADIOLIB_TEST_TX" for transmit mode
# Default: receive mode

include $(RIOTBASE)/Makefile.include
