BOARD ?= slstk3401a
include ../Makefile.tests_common

# method `fflush()` is not defined for MSP-430 (#6445 will fix this)
BOARD_BLACKLIST := chronos msb-430h msb-430 telosb z1

FEATURES_OPTIONAL += periph_rtc
FEATURES_OPTIONAL += periph_gpio_irq

USEMODULE += shell
USEMODULE += shell_commands

include $(RIOTBASE)/Makefile.include
