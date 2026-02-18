# name of the application
APPLICATION = home-assistant-integration

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

# Include packages that pull up and auto-init the link layer.
# NOTE: 6LoWPAN will be included if IEEE802.15.4 devices are present
USEMODULE += auto_init_gnrc_netif
USEMODULE += gnrc_ipv6_router_default
USEMODULE += gnrc_icmpv6_echo
USEMODULE += usbus_cdc_ecm
#USEMODULE += netdev_default
USEMODULE += gnrc_icmpv6_error
USEMODULE += shell
USEMODULE += shell_cmds_default
USEMODULE += luid
USEMODULE += fmt
USEMODULE += saul_reg
USEMODULE += saul_default
USEMODULE += ps

USEMODULE += sock_async_event
USEMODULE += sock_ip
USEMODULE += sock_udp
USEMODULE += sock_tcp

# required packages
USEPKG += paho-mqtt
USEPKG += mjson


# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

include $(RIOTBASE)/Makefile.include
