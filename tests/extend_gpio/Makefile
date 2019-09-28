include ../Makefile.tests_common

# The CPUs of these boards do not provide a gpio.c API.
# CPUs: cc430, mips32r2_generic, native
BOARD_BLACKLIST := chronos mips-malta native

BOARD_INSUFFICIENT_MEMORY := arduino-duemilanove arduino-uno arduino-nano

FEATURES_REQUIRED += periph_gpio_irq

USEMODULE += extend_gpio

include ./Makefile.include
include $(RIOTBASE)/Makefile.include
