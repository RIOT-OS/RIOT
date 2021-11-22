# name of your application
APPLICATION = minmea_uart

# Use the Seeedstudio LoRa E5 Dev or Mini board by default:
BOARD ?= lora-e5-dev

FEATURES_REQUIRED += periph_uart
FEATURES_OPTIONAL += periph_lpuart  # STM32 L0 and L4 provides lpuart support
FEATURES_OPTIONAL += periph_uart_modecfg
FEATURES_OPTIONAL += periph_uart_rxstart_irq

USEMODULE += shell
USEMODULE += xtimer

USEPKG += minmea
USEMODULE += fmt
#USEMODULE += printf_float

#CFLAGS += -DISR_STACKSIZE=2048U

CFLAGS += -Wno-switch

CFLAGS += -DSTDIO_UART_BAUDRATE=9600

# avoid running Kconfig by default
SHOULD_RUN_KCONFIG ?=

include $(RIOTBASE)/Makefile.include
