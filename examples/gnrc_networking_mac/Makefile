# name of your application
APPLICATION = gnrc_networking_mac

# If no BOARD is found in the environment, use this default:
BOARD ?= samr21-xpro

# Currently, GoMacH has only been tested and evaluated through on samr21-xpro, iotlab-m3 and
# derfmega256 (at86rf2xx based radios).
# Once GoMacH has also been tested through on other boards, the whitelist should be
# then accordingly extended.
# Notably, for LWMAC, we have only evaluated it on samr21-xpro nodes. To this end, if
# you are going to include LWMAC for testing, you should only run the example on samr21-xpro.
BOARD_WHITELIST := samr21-xpro iotlab-m3 avr-rss2 atmega256rfr2-xpro derfmega256

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

# Include packages that pull up and auto-init the link layer.
# NOTE: 6LoWPAN will be included if IEEE802.15.4 devices are present
USEMODULE += gnrc_netdev_default
USEMODULE += auto_init_gnrc_netif
# Specify the mandatory networking modules for IPv6 and UDP
USEMODULE += gnrc_ipv6_router_default
USEMODULE += gnrc_udp
# Add a routing protocol
USEMODULE += gnrc_rpl
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
# Use GoMacH as the MAC layer protocol
USEMODULE += gnrc_gomach
# In case of using LWMAC MAC protocol instead of GoMacH, uncomment the following line and comment the above line
# USEMODULE += gnrc_lwmac

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

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
