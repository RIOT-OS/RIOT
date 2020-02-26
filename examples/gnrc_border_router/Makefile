# name of your application
APPLICATION = gnrc_border_router

# If no BOARD is found in the environment, use this default:
BOARD ?= samr21-xpro

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

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

# Optionally use DHCPv6 instead of UHCP
USE_DHCPV6 ?= 0

ifeq (1,$(USE_DHCPV6))
  # include DHCPv6 client for 6LoWPAN border router
  USEMODULE += gnrc_dhcpv6_client_6lbr
else
  # include UHCP client
  USEMODULE += gnrc_uhcpc
endif

# Optionally use SLIP instead of ethos when not on native
# (added to USEMODULE in Makefile.board.dep)
USE_SLIP ?= 0

# Use two network interfaces
GNRC_NETIF_NUMOF := 2

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

# Ethos/native TAP interface and UHCP prefix can be configured from make command
TAP ?= tap0
IPV6_PREFIX ?= 2001:db8::/64

# MODULE DEPENDENT CONFIGURATION IMPORT
# =====================================
# use ethos (ethernet over serial) or SLIP (serial-line IP) for network
# communication and stdio over UART, but not on native, as native has a tap
# interface towards the host.
ifeq (,$(filter native,$(BOARD)))
  ifeq (1,$(USE_SLIP))
    # SLIP baudrate and UART device can be configured from make command
    SLIP_UART ?= "UART_DEV(UART_NUMOF-1)"
    SLIP_BAUDRATE ?= 115200
    include $(CURDIR)/Makefile.slip.conf
  else
    # ethos baudrate can be configured from make command
    ETHOS_BAUDRATE ?= 115200
    include $(CURDIR)/Makefile.ethos.conf
  endif
else
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
	$(Q)env -u CC -u CFLAGS make -C $(RIOTTOOLS)

# define native specific targets to only run UHCP daemon when required
ifneq (,$(filter native,$(BOARD)))
ifneq (1,$(USE_DHCPV6))
.PHONY: uhcpd-daemon

uhcpd-daemon: host-tools
	$(RIOTTOOLS)/uhcpd/bin/uhcpd $(TAP) $(IPV6_PREFIX) &
endif
endif

# Set a custom channel if needed
include $(RIOTMAKE)/default-radio-settings.inc.mk
