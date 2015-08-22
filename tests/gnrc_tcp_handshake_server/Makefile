# name of your application
APPLICATION = handshake_server

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
CFLAGS += -DDEVELHELP

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

# Modules to include
USEMODULE += vtimer
USEMODULE += random

USEMODULE += gnrc_netif_default
USEMODULE += auto_init_ng_netif

USEMODULE += gnrc_ipv6_default
USEMODULE += gnrc_tcp

USEMODULE += gnrc_pktdump
USEMODULE += gnrc_icmpv6_echo

include $(RIOTBASE)/Makefile.include
