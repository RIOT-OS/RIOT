# name of your application
APPLICATION = posix_sockets_example

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../../../..

# Include packages that pull up and auto-init the link layer.
# NOTE: 6LoWPAN will be included if IEEE802.15.4 devices are present
USEMODULE += netdev_default
USEMODULE += auto_init_gnrc_netif
# Specify the mandatory networking modules for socket communication via UDP
USEMODULE += gnrc_ipv6_default
USEMODULE += sock_udp
USEMODULE += posix_sockets
USEMODULE += posix_sleep
USEMODULE += posix_inet
# Add also the shell, some shell commands
USEMODULE += shell_cmds_default
USEMODULE += ps

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

include $(RIOTBASE)/Makefile.include
