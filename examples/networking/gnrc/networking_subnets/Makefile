# name of your application
APPLICATION = gnrc_networking-subnets

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../../../..

# Include packages that pull up and auto-init the link layer.
USEMODULE += netdev_default
USEMODULE += auto_init_gnrc_netif
# Activate ICMPv6 error messages
USEMODULE += gnrc_icmpv6_error
# Specify the mandatory networking modules for IPv6 and UDP
USEMODULE += gnrc_udp
# This application dumps received packets to STDIO using the pktdump module
USEMODULE += gnrc_pktdump
# Additional networking modules that can be dropped if not needed
USEMODULE += gnrc_icmpv6_echo
# Add also the shell, some shell commands
USEMODULE += shell_cmds_default
USEMODULE += ps
USEMODULE += netstats_l2
USEMODULE += netstats_ipv6

# leaf nodes only have a single interface
ifeq (1, $(LEAF))
  APPLICATION := $(APPLICATION)_leaf
  USEMODULE += gnrc_ipv6_default
else
  USEMODULE += gnrc_ipv6_router_default
  USEMODULE += gnrc_ipv6_auto_subnets
  CFLAGS += -DNETDEV_TAP_MAX=2
  PORT ?= tap_a0 tap_b0
endif

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

include $(RIOTBASE)/Makefile.include

# Set a custom channel if needed
include $(RIOTMAKE)/default-radio-settings.inc.mk
