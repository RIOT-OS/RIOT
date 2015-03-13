APPLICATION = driver_xbee
include ../Makefile.tests_common

FEATURES_REQUIRED = periph_uart periph_gpio

BOARD_INSUFFICIENT_RAM := stm32f0discovery

# Define default pin mappings for some boards:
ifneq (,$(filter stm32f0discovery,$(BOARD)))
  export XBEE_UART ?= UART_1
endif
ifneq (,$(filter stm32f4discovery,$(BOARD)))
  export XBEE_UART ?= UART_1
endif
ifneq (,$(filter nucleo-f091,$(BOARD)))
  export XBEE_UART ?= UART_1
endif

USEMODULE += xbee
USEMODULE += ng_netbase
USEMODULE += ng_nomac
USEMODULE += ng_pktdump
USEMODULE += shell
USEMODULE += shell_commands

ifneq (,$(XBEE_UART))
  CFLAGS += -DXBEE_UART=$(XBEE_UART)
else
  # set default
  CFLAGS += -DXBEE_UART=UART_0
endif

include $(RIOTBASE)/Makefile.include
