include ../Makefile.net_common

USEMODULE += auto_init_gnrc_netif
USEMODULE += gnrc
USEMODULE += gnrc_ipv6
USEMODULE += gnrc_icmpv6_echo
USEMODULE += gnrc_pktdump
USEMODULE += gnrc_txtsnd
USEMODULE += slipmux_net
USEMODULE += shell
USEMODULE += shell_cmds_default

# set slip parameters to default values if unset
SLIP_UART     ?= "UART_NUMOF-1"
SLIP_BAUDRATE ?= 115200

# export slip parameters
CFLAGS += -DSLIPMUX_PARAM_UART="UART_DEV($(SLIP_UART))"
CFLAGS += -DSLIPMUX_PARAM_BAUDRATE=$(SLIP_BAUDRATE)

# add current directory to the include path. Putting it in CFLAGS will make
# it go to the beginning, before the standard includes.
# That way slip_params.h get's included and auto configuration can pick it up.
CFLAGS += -I$(CURDIR)

include $(RIOTBASE)/Makefile.include
