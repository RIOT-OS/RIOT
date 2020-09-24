include ../Makefile.tests_common

FEATURES_REQUIRED += periph_uart
FEATURES_OPTIONAL += periph_lpuart  # STM32 L0 and L4 provides lpuart support
FEATURES_OPTIONAL += periph_uart_modecfg

USEMODULE += shell
USEMODULE += xtimer

# avoid running Kconfig by default
SHOULD_RUN_KCONFIG ?=

include $(RIOTBASE)/Makefile.include
