BOARD ?= atmega328p
include ../Makefile.tests_common

# Update this to your needs
PIN ?= GPIO_PIN(0, 0)
N ?= 8

USEMODULE += ws281x

FEATURES_REQUIRED := arch_avr8 # <-- Currently only backend for AVR boards

# Only AVR boards CPU clocked at 8MHz or 16 MHz are supported. The Waspmote Pro
# is clocked at 14.7456 MHz :-/
BOARD_BLACKLIST := waspmote-pro

include $(RIOTBASE)/Makefile.include

CFLAGS += '-DWS281X_PARAM_PIN=$(PIN)'
CFLAGS += '-DWS281X_PARAM_NUMOF=$(N)'
