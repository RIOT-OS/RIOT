# name of your application
APPLICATION = gnrc_networking-subnets

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

# Include packages that pull up and auto-init the link layer.
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
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps
USEMODULE += netstats_l2
USEMODULE += netstats_ipv6

CFLAGS += -DDEBUG_ASSERT_VERBOSE=1
CFLAGS += -DCONFIG_GNRC_IPV6_NIB_NUMOF=16
CFLAGS += -DCONFIG_GNRC_IPV6_NIB_ADV_ROUTER=0

ifeq (native, $(BOARD))
  USEMODULE += netdev_tap
  CFLAGS += -DCONFIG_GNRC_DHCPV6_CLIENT_6LBR_UPSTREAM=6
else ifeq (1, $(SLIP))
  USEMODULE += slipdev_l2addr
  CFLAGS += -DCONFIG_GNRC_DHCPV6_CLIENT_6LBR_UPSTREAM=5
else
  USEMODULE += ethos
  CFLAGS += -DCONFIG_GNRC_DHCPV6_CLIENT_6LBR_UPSTREAM=6
endif

CFLAGS += -DSLIPDEV_PARAM_UART=UART_DEV\(1\)
CFLAGS += -DETHOS_UART=UART_DEV\(1\)
CFLAGS += -DETHOS_BAUDRATE=115200

# leaf nodes only have a single interface
ifeq (1, $(LEAF))
  APPLICATION := $(APPLICATION)_leaf
  USEMODULE += gnrc_ipv6_default
else
  USEMODULE += gnrc_ipv6_router_default
  CFLAGS += -DNETDEV_TAP_MAX=2
endif

# gateway gets prefix via DHCPv6
ifeq (1, $(GATEWAY))
  APPLICATION := $(APPLICATION)_gateway

  USEMODULE += gnrc_netdev_default
  USEMODULE += gnrc_dhcpv6_client_simple_pd
else ifneq (1, $(LEAF))
  USEMODULE += gnrc_ipv6_nib_subnets
endif

# Optionally include DNS support. This includes resolution of names at an
# upstream DNS server and the handling of RDNSS options in Router Advertisements
# to auto-configure that upstream DNS server.
USEMODULE += sock_dns              # include DNS client
USEMODULE += gnrc_ipv6_nib_dns     # include RDNSS option handling

# Enable handling of the Route Information Option
USEMODULE += gnrc_ipv6_nib_rio

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

# Uncomment the following 2 lines to specify static link lokal IPv6 address
# this might be useful for testing, in cases where you cannot or do not want to
# run a shell with ifconfig to get the real link lokal address.
#IPV6_STATIC_LLADDR ?= '"fe80::cafe:cafe:cafe:1"'
#CFLAGS += -DGNRC_IPV6_STATIC_LLADDR=$(IPV6_STATIC_LLADDR)

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

include $(RIOTBASE)/Makefile.include

# Set a custom channel if needed
include $(RIOTMAKE)/default-radio-settings.inc.mk
