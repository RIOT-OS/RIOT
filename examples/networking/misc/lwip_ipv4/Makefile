# Set the name of your application:
APPLICATION = lwip_ipv4

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../../../../

# Uncomment this to enable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP = 1

# Change this to 0 to show compiler invocation lines by default:
QUIET ?= 1

# Modules to include:
USEMODULE += shell

USEMODULE += netdev_default

#enables LWIP IPv4 stack
USEMODULE += ipv4_addr
USEMODULE += lwip_arp
USEMODULE += lwip_ipv4

# If this module will be disabled, example set IPv4 192.168.100.11 address.
# Enable next two line to use DHCP
#USEMODULE += lwip_dhcp_auto
#CFLAGS += -DETHARP_SUPPORT_STATIC_ENTRIES=1

#enables UDP LWIP sockets
USEMODULE += sock_udp

#nables ifconfig command, which allows IPv4 address configuration
USEMODULE += shell_cmds_default

include $(RIOTBASE)/Makefile.include
