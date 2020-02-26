BOARD ?= atmega328p
include ../Makefile.tests_common

# Update this to your needs
PIN ?= GPIO_PIN(0, 0)
N ?= 8

USEMODULE += ws281x

# Currently the ws281x only supports AVR-based platforms and native
# (via VT100 terminals).
# See https://doc.riot-os.org/group__drivers__ws281x.html
FEATURES_BLACKLIST += arch_arm
FEATURES_BLACKLIST += arch_esp32
FEATURES_BLACKLIST += arch_esp8266
FEATURES_BLACKLIST += arch_mips32r2
FEATURES_BLACKLIST += arch_msp430
FEATURES_BLACKLIST += arch_riscv

# Only AVR boards CPU clocked at 8MHz or 16 MHz are supported. The Waspmote Pro
# is clocked at 14.7456 MHz :-/
BOARD_BLACKLIST := waspmote-pro

include $(RIOTBASE)/Makefile.include

CFLAGS += '-DWS281X_PARAM_PIN=$(PIN)'
CFLAGS += '-DWS281X_PARAM_NUMOF=$(N)'
