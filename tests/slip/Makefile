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

# add current directory to the include path. Putting it in CFLAGS will make
# it go to the beginning, before the standard includes.
# That way slip_params.h get's included and auto configuration can pick it up.
CFLAGS += -I$(CURDIR)

include $(RIOTBASE)/Makefile.include
