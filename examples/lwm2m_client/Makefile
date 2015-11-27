# name of your application
APPLICATION = lwm2m_client

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

# include the shell and shell commands
USEMODULE += shell
USEMODULE += shell_commands
# include the LWM2M client adaption layer
USEMODULE += lwm2m_cli
# pull in and initialize default network interface(s)
USEMODULE += gnrc_netif_default
USEMODULE += auto_init_gnrc_netif
# for debugging and testing purposes
USEMODULE += gnrc_icmpv6_echo
USEMODULE += ps

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
CFLAGS += -DDEVELHELP

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

include $(RIOTBASE)/Makefile.include
