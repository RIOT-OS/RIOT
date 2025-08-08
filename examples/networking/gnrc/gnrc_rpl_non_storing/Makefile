# name of your application
APPLICATION = gnrc_rpl_non_storing

# If no BOARD is found in the environment, use this default:
BOARD ?= native
# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../../../..

# Include packages that pull up and auto-init the link layer.
# NOTE: 6LoWPAN will be included if IEEE802.15.4 devices are present
USEMODULE += netdev_default
USEMODULE += auto_init_gnrc_netif
# Activate ICMPv6 error messages
USEMODULE += gnrc_icmpv6_error
# Specify the mandatory networking module for a IPv6 routing node
USEMODULE += gnrc_ipv6_router_default
# Add a routing protocol
USEMODULE += gnrc_rpl
USEMODULE += auto_init_gnrc_rpl
# Additional networking modules that can be dropped if not needed
USEMODULE += gnrc_icmpv6_echo
USEMODULE += shell_cmd_gnrc_udp
# Add also the shell, some shell commands
USEMODULE += shell
USEMODULE += shell_cmds_default
USEMODULE += ps
USEMODULE += netstats_l2
USEMODULE += netstats_ipv6
USEMODULE += netstats_rpl

# Set RPL Non-storing mode
# ** To set RPL to Storing mode use instead: CFLAGS += -DCONFIG_GNRC_RPL_MOP_STORING_MODE=1
CFLAGS += -DCONFIG_GNRC_RPL_MOP_NON_STORING_MODE=1

# allow to use large blocks to utilize large MTUs (802.15.4g, Ethernet, WiFi)
LARGE_BLOCKS ?= 0
ifeq (1, $(LARGE_BLOCKS))
  CFLAGS += -DCONFIG_SUIT_COAP_BLOCKSIZE=COAP_BLOCKSIZE_1024
endif


DEVELHELP ?= 1

# Instead of simulating an Ethernet connection, we can also simulate
# an IEEE 802.15.4 radio using ZEP
USE_ZEP ?= 0

# set the ZEP port for native
ZEP_PORT_BASE ?= 17754
ifeq (1,$(USE_ZEP))
  TERMFLAGS += -z [::1]:$(ZEP_PORT_BASE)
  USEMODULE += socket_zep
endif

# Uncomment the following 2 lines to specify static link lokal IPv6 address
# this might be useful for testing, in cases where you cannot or do not want to
# run a shell with ifconfig to get the real link lokal address.
#IPV6_STATIC_LLADDR ?= '"fe80::cafe:cafe:cafe:1"'
#CFLAGS += -DGNRC_IPV6_STATIC_LLADDR=$(IPV6_STATIC_LLADDR)


# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1
DEFAULT_CHANNEL=15
include $(RIOTBASE)/Makefile.include

# Set a custom channel if needed
include $(RIOTMAKE)/default-radio-settings.inc.mk
