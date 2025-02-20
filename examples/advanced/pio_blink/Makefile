APPLICATION = pio_blink
BOARD ?= rpi-pico
RIOTBASE ?= $(CURDIR)/../../..
# The board rpi-pico-w has no LED pin so you have to select the pin by hand with:
# PIO_BLINK_PIN=GPIO_PIN\(x,y\) make ...
PIO_BLINK_PIN ?= GPIO_UNDEF

CFLAGS += -DPIO_BLINK_PIN=$(PIO_BLINK_PIN)

FEATURES_REQUIRED += periph_pio

DEVELHELP ?= 1
QUIET ?= 1

include $(RIOTBASE)/Makefile.include

DIRS += $(CPU)_pio_blink
USEMODULE += $(CPU)_pio_blink
