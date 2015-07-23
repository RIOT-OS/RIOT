# name of your application
APPLICATION = handshake

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
USEMODULE += ng_netif_default
USEMODULE += auto_init_ng_netif
USEMODULE += ng_ipv6_default
USEMODULE += ng_tcp
USEMODULE += od

CFLAGS += -DDEVELHELP

include $(RIOTBASE)/Makefile.include
