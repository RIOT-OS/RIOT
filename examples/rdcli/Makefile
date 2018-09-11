# name of your application
APPLICATION = rdcli

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

USEMODULE += gnrc_netdev_default
USEMODULE += auto_init_gnrc_netif
USEMODULE += gnrc_ipv6_default
USEMODULE += gnrc_icmpv6_echo

USEMODULE += rdcli_standalone

USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps
USEMODULE += fmt

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
CFLAGS += -DDEVELHELP

# For debugging and demonstration purposes, we limit the lifetime to 60s
CFLAGS += -DRDCLI_LT=60

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

include $(RIOTBASE)/Makefile.include
