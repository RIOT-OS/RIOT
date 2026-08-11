
MODULE_DEFS_NEEDED += \
  periph_uart_collision \
  periph_uart_modecfg \
  periph_uart_rxstart_irq \
  stdio_uart \
  ztimer \
  #
include ../Makefile.periph_common

FEATURES_REQUIRED += periph_uart
FEATURES_OPTIONAL += periph_lpuart  # STM32 L0 and L4 provides lpuart support
FEATURES_OPTIONAL += periph_uart_collision
FEATURES_OPTIONAL += periph_uart_modecfg
FEATURES_OPTIONAL += periph_uart_rxstart_irq
FEATURES_OPTIONAL += periph_timer   # Use the timer if available

USEMODULE += bitfield
USEMODULE += shell

# avoid running Kconfig by default
SHOULD_RUN_KCONFIG ?=

include $(RIOTBASE)/Makefile.include
