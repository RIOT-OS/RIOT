# Name of your application
APPLICATION = coap_hello_client

# Change this to your board if you want to build for a different board
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
# If you are following the tutorial, your RIOT base directory will
# most likely be something like RIOTBASE ?= $(CURDIR)/RIOT
# instead of this
RIOTBASE ?= $(CURDIR)/../../../..

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

# Include packages that pull up and auto-init the link layer
USEMODULE += netdev_default
USEMODULE += auto_init_gnrc_netif

# Network stack
USEMODULE += gnrc_ipv6_default
USEMODULE += gnrc_icmpv6_echo

# Include gcoap module
USEMODULE += gcoap

# Include shell for interactive commands
USEMODULE += shell
USEMODULE += shell_cmds_default

# CoAP Server Address
CFLAGS += -DSERVER_ADDRESS='"fe80::cafe:cafe:cafe:1"'

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

include $(RIOTBASE)/Makefile.include
