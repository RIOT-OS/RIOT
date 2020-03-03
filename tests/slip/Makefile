include ../Makefile.tests_common

USEMODULE += auto_init_gnrc_netif
USEMODULE += gnrc
USEMODULE += gnrc_pktdump
USEMODULE += gnrc_txtsnd
USEMODULE += slipdev
USEMODULE += shell
USEMODULE += shell_commands

# set slip parameters to default values if unset
SLIP_UART     ?= "UART_NUMOF-1"
SLIP_BAUDRATE ?= 115200

# export slip parameters
CFLAGS += -DSLIP_UART="UART_DEV($(SLIP_UART))"
CFLAGS += -DSLIP_BAUDRATE=$(SLIP_BAUDRATE)

include $(RIOTBASE)/Makefile.include
