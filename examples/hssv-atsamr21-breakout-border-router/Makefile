#+-------------------------------------------------------------------------------------------------+
#| GNU Make script for the border router based on hssv-atsamr21-breakout with CDC-ECM.             |
#|                                                                                                 |
#| This is based on the Makefile from the gnrc_boder_router example.                               |
#| Modified by: Joksan Alvarado.                                                                   |
#+-------------------------------------------------------------------------------------------------+

# name of your application
APPLICATION = hssv_atsamr21_breakout_border_router

# If no BOARD is found in the environment, use this default:
BOARD := hssv-atsamr21-breakout

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

# Include the USB CDC-ECM network interface. Adjust the amount of network interfaces accordingly.
USEMODULE += usbus_cdc_ecm
GNRC_NETIF_NUMOF := 2

# Include packages that pull up and auto-init the link layer.
# NOTE: 6LoWPAN will be included if IEEE802.15.4 devices are present
USEMODULE += gnrc_netdev_default
USEMODULE += auto_init_gnrc_netif
# Specify the mandatory networking modules for 6LoWPAN border router
USEMODULE += gnrc_sixlowpan_border_router_default
# Add forwarding table
USEMODULE += fib
# Additional networking modules that can be dropped if not needed
USEMODULE += gnrc_icmpv6_echo
# Add also the shell, some shell commands
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps

# include UHCP client
USEMODULE += gnrc_uhcpc

# Optionally include RPL as a routing protocol. When includede gnrc_uhcpc will
# configure the node as a RPL DODAG root when receiving a prefix.
#USEMODULE += gnrc_rpl

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

# radio is IEEE 802.15.4 2.4 GHz
DEFAULT_CHANNEL ?= 26
CFLAGS += -DIEEE802154_DEFAULT_CHANNEL=$(DEFAULT_CHANNEL)
# CFLAGS += -DIEEE802154_DEFAULT_PANID=0xYOUR_NETWORK_ID

include $(RIOTBASE)/Makefile.include

.PHONY: host-tools

host-tools:
	$(Q)env -u CC -u CFLAGS make -C $(RIOTTOOLS)
