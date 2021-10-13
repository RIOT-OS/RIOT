# name of your application
APPLICATION = cord_ep

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

USEMODULE += netdev_default
USEMODULE += auto_init_gnrc_netif
USEMODULE += gnrc_ipv6_default
USEMODULE += gnrc_icmpv6_echo

USEMODULE += cord_ep_standalone

USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps
USEMODULE += fmt

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

include $(RIOTBASE)/Makefile.include

# For debugging and demonstration purposes, we limit the lifetime to 60s
# Set CONFIG_CORD_LT only if not being set via Kconfig
ifndef CONFIG_CORD_LT
CFLAGS += -DCONFIG_CORD_LT=60
endif
