APPLICATION = driver_xbee
include ../Makefile.tests_common

FEATURES_REQUIRED = periph_uart periph_gpio

BOARD_INSUFFICIENT_MEMORY := nucleo-f030 nucleo-f334 stm32f0discovery weio \
                             nucleo-f042

USEMODULE += xbee
USEMODULE += gnrc_netif
USEMODULE += gnrc_netdev2
USEMODULE += auto_init_gnrc_netif
USEMODULE += gnrc_pktdump
USEMODULE += shell
USEMODULE += shell_commands

# set default UART to use in case none was defined
XBEE_UART ?= "UART_NUMOF-1"

# export UART to params file
CFLAGS += -DXBEE_UART=$(XBEE_UART)

# add current directory to the include path. Putting it in CFLAGS will make
# it go to the beginning, before the standard includes.
# That way xbee_params.h get's included and auto configuration can pick it up.
CFLAGS += -I$(CURDIR)

include $(RIOTBASE)/Makefile.include
