BOARD ?= slstk3401a
include ../Makefile.tests_common

FEATURES_OPTIONAL += periph_rtc
FEATURES_OPTIONAL += periph_gpio_irq

USEMODULE += shell
USEMODULE += shell_commands

# avoid running Kconfig by default
SHOULD_RUN_KCONFIG ?=

include $(RIOTBASE)/Makefile.include
