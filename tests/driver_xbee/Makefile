APPLICATION = driver_xbee
include ../Makefile.tests_common

FEATURES_REQUIRED = periph_uart periph_gpio

BOARD_INSUFFICIENT_MEMORY := stm32f0discovery

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
USEMODULE += gnrc_netif
USEMODULE += auto_init_gnrc_netif
USEMODULE += gnrc_nomac
USEMODULE += gnrc_pktdump
USEMODULE += shell
USEMODULE += shell_commands

ifneq (,$(XBEE_UART))
  CFLAGS += -DXBEE_UART=$(XBEE_UART)
else
  # set default
  CFLAGS += -DXBEE_UART=UART_0
endif

# add current directory to the include path. Putting it in CFLAGS will make
# it go to the beginning, before the standard includes.
# That way xbee_params.h get's included and auto configuration can pick it up.
CFLAGS += -I$(CURDIR)

include $(RIOTBASE)/Makefile.include
