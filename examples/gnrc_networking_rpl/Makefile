# name of your application
APPLICATION = gnrc_rpl_demo

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

# we need to define the interface on which RPL will run
ifeq (native,$(BOARD))
  CFLAGS += -DCONFIG_GNRC_RPL_DEFAULT_NETIF=7
else
  CFLAGS += -DCONFIG_GNRC_RPL_DEFAULT_NETIF=6
endif

# foren6 sniffer listens on channel 11
CFLAGS += -DCONFIG_IEEE802154_DEFAULT_CHANNEL=11

# enable test with a medium sized mesh of 50 nodes
CFLAGS += -DCONFIG_GNRC_IPV6_NIB_OFFL_NUMOF=50
CFLAGS += -DCONFIG_GNRC_IPV6_NIB_NUMOF=50
CFLAGS += -DNETSTATS_NB_SIZE=32

# Include packages that pull up and auto-init the link layer.
# NOTE: 6LoWPAN will be included if IEEE802.15.4 devices are present
USEMODULE += gnrc_netdev_default
USEMODULE += auto_init_gnrc_netif
# Activate ICMPv6 error messages
USEMODULE += gnrc_icmpv6_error
# Specify the mandatory networking modules for IPv6 and UDP
USEMODULE += gnrc_ipv6_router_default
USEMODULE += gnrc_udp
# Add a routing protocol
USEMODULE += gnrc_rpl
USEMODULE += gnrc_rpl_mrhof
# optional alternative metric, hardware must support it
# USEMODULE += gnrc_rpl_mrhof_lqi
USEMODULE += auto_init_gnrc_rpl
# This application dumps received packets to STDIO using the pktdump module
USEMODULE += gnrc_pktdump
# Additional networking modules that can be dropped if not needed
USEMODULE += gnrc_icmpv6_echo
# Add also the shell, some shell commands
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps
USEMODULE += netstats_l2
USEMODULE += netstats_ipv6
USEMODULE += netstats_rpl

# For the demo, include all statistics modules
USEMODULE += netstats_neighbor_etx
USEMODULE += netstats_neighbor_count
USEMODULE += netstats_neighbor_rssi
USEMODULE += netstats_neighbor_lqi
USEMODULE += netstats_neighbor_tx_time

# Optionally include DNS support. This includes resolution of names at an
# upstream DNS server and the handling of RDNSS options in Router Advertisements
# to auto-configure that upstream DNS server.
# USEMODULE += sock_dns              # include DNS client
# USEMODULE += gnrc_ipv6_nib_dns     # include RDNSS option handling

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

# set the ZEP port for native
ZEP_PORT_BASE ?= 17754
ifeq (native,$(BOARD))
  TERMFLAGS += -z [::1]:$(ZEP_PORT_BASE)
  USEMODULE += socket_zep
endif

# Uncomment the following 2 lines to specify static link lokal IPv6 address
# this might be useful for testing, in cases where you cannot or do not want to
# run a shell with ifconfig to get the real link lokal address.
#IPV6_STATIC_LLADDR ?= '"fe80::cafe:cafe:cafe:1"'
#CFLAGS += -DGNRC_IPV6_STATIC_LLADDR=$(IPV6_STATIC_LLADDR)

# Uncomment this to join RPL DODAGs even if DIOs do not contain
# DODAG Configuration Options (see the doc for more info)
# CFLAGS += -DCONFIG_GNRC_RPL_DODAG_CONF_OPTIONAL_ON_JOIN

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

include $(RIOTBASE)/Makefile.include

# Set a custom channel if needed
include $(RIOTMAKE)/default-radio-settings.inc.mk
