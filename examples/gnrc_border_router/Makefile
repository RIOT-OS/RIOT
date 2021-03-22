# name of your application
APPLICATION = gnrc_border_router

# If no BOARD is found in the environment, use this default:
BOARD ?= samr21-xpro

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

# Default to using ethos for providing the uplink when not on native
UPLINK ?= ethos

# Check if the selected Uplink is valid
ifeq (,$(filter ethos slip cdc-ecm wifi ethernet,$(UPLINK)))
  $(error Supported uplinks are `ethos`, `slip`, `cdc-ecm`, `ethernet` and `wifi`)
endif

# Set the SSID and password of your WiFi network here
WIFI_SSID ?= "Your_WiFi_name"
WIFI_PASS ?= "Your_secure_password"

# Include packages that pull up and auto-init the link layer.
# NOTE: 6LoWPAN will be included if IEEE802.15.4 devices are present
USEMODULE += gnrc_netdev_default
USEMODULE += auto_init_gnrc_netif
# Specify the mandatory networking modules for 6LoWPAN border router
USEMODULE += gnrc_sixlowpan_border_router_default
# Additional networking modules that can be dropped if not needed
USEMODULE += gnrc_icmpv6_echo
# Add also the shell, some shell commands
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps

# Optionally include RPL as a routing protocol. When includede gnrc_uhcpc will
# configure the node as a RPL DODAG root when receiving a prefix.
#USEMODULE += gnrc_rpl

# Optionally include DNS support. This includes resolution of names at an
# upstream DNS server and the handling of RDNSS options in Router Advertisements
# to auto-configure that upstream DNS server.
#USEMODULE += sock_dns              # include DNS client
#USEMODULE += gnrc_ipv6_nib_dns     # include RDNSS option handling

# When using a WiFi uplink we should use DHCPv6
ifneq (,$(filter cdc-ecm wifi ethernet,$(UPLINK)))
  USE_DHCPV6 ?= 1
else
  USE_DHCPV6 ?= 0
endif

ifeq (1,$(USE_DHCPV6))
  # include DHCPv6 client for 6LoWPAN border router
  USEMODULE += gnrc_dhcpv6_client_6lbr
else
  # include UHCP client
  USEMODULE += gnrc_uhcpc
endif

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

# Ethos/native TAP interface and UHCP prefix can be configured from make command
TAP ?= tap0
ifeq (1,$(USE_DHCPV6))
  # with DHCPv6 the 64-bit delegated prefixes are generated from a shorter
  # configured prefix.
  IPV6_PREFIX ?= 2001:db8::/32
else
  # UHCP advertises the prefix as is.
  IPV6_PREFIX ?= 2001:db8::/64
endif

# MODULE DEPENDENT CONFIGURATION IMPORT
# =====================================
# use ethos (ethernet over serial) or SLIP (serial-line IP) for network
# communication and stdio over UART, but not on native, as native has a tap
# interface towards the host.
ifeq (,$(filter native,$(BOARD)))
  ifeq (slip,$(UPLINK))
    # SLIP baudrate and UART device can be configured from make command
    SLIP_BAUDRATE ?= 115200
    include $(CURDIR)/Makefile.slip.conf
  else ifeq (ethos,$(UPLINK))
    # ethos baudrate can be configured from make command
    ETHOS_BAUDRATE ?= 115200
    include $(CURDIR)/Makefile.ethos.conf
  else ifeq (cdc-ecm,$(UPLINK))
    include $(CURDIR)/Makefile.cdc-ecm.conf
  else ifeq (wifi,$(UPLINK))
    # SSID and Password need to be configured
    include $(CURDIR)/Makefile.wifi.conf
  endif
else
  # The number of native (emulated) ZigBee/6LoWPAN devices
  ZEP_DEVICES ?= 1
  include $(CURDIR)/Makefile.native.conf
endif

# As there is an 'Kconfig' we want to explicitly disable Kconfig by setting
# the variable to empty
SHOULD_RUN_KCONFIG ?=

include $(RIOTBASE)/Makefile.include

# Compile-time configuration for DHCPv6 client (needs to come after
# Makefile.include as this might come from Kconfig)
ifeq (1,$(USE_DHCPV6))
  ifndef CONFIG_GNRC_DHCPV6_CLIENT_6LBR_STATIC_ROUTE
    ifeq (1,$(STATIC_ROUTES))
      CFLAGS += -DCONFIG_GNRC_DHCPV6_CLIENT_6LBR_STATIC_ROUTE=1
      # CONFIG_GNRC_DHCPV6_CLIENT_6LBR_STATIC_ROUTE=1 requires one more address
      # for `fe80::2`.
      CFLAGS += -DCONFIG_GNRC_NETIF_IPV6_ADDRS_NUMOF=3
    endif
  endif
endif


.PHONY: host-tools

host-tools:
	$(Q)env -u CC -u CFLAGS $(MAKE) -C $(RIOTTOOLS)

# define native specific targets to only run UHCP daemon when required
ifneq (,$(filter native,$(BOARD)))
ifneq (1,$(USE_DHCPV6))
.PHONY: uhcpd-daemon

uhcpd-daemon: host-tools
	$(RIOTTOOLS)/uhcpd/bin/uhcpd $(TAP) $(IPV6_PREFIX) &
endif
endif

ifeq (slip,$(UPLINK))
sliptty:
	$(Q)env -u CC -u CFLAGS $(MAKE) -C $(RIOTTOOLS)/sliptty
endif

# Set a custom channel if needed
include $(RIOTMAKE)/default-radio-settings.inc.mk
