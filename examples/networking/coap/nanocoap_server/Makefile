# name of your application
APPLICATION = nanocoap_server

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../../../..

# use GNRC by default
LWIP_IPV4 ?= 0
LWIP_IPV6 ?= 0

# Include packages that pull up and auto-init the link layer.
# NOTE: 6LoWPAN will be included if IEEE802.15.4 devices are present
USEMODULE += netdev_default

ifeq (,$(filter 1, $(LWIP_IPV4) $(LWIP_IPV6)))
  USEMODULE += auto_init_gnrc_netif
  # Specify the mandatory networking modules
  USEMODULE += gnrc_ipv6_default
  # Additional networking modules that can be dropped if not needed
  USEMODULE += gnrc_icmpv6_echo
else
  ifeq (1,$(LWIP_IPV4))
    USEMODULE += ipv4_addr

    USEMODULE += lwip_arp
    USEMODULE += lwip_ipv4

# If this module will be disabled, example set IPv4 192.168.100.150 address.
    USEMODULE += lwip_dhcp_auto
    CFLAGS += -DETHARP_SUPPORT_STATIC_ENTRIES=1

    USEMODULE += random
  endif

  ifeq (1,$(LWIP_IPV6))
    USEMODULE += ipv6_addr

    USEMODULE += lwip_ipv6
    USEMODULE += lwip_ipv6_autoconfig
  endif
endif

USEMODULE += sock_udp

USEMODULE += nanocoap_sock
USEMODULE += sock_udp
USEMODULE += nanocoap_resources
USEMODULE += nanocoap_sock
USEMODULE += nanocoap_udp

ifeq (,$(filter 1, $(LWIP_IPV4) $(LWIP_IPV6)))
  USEMODULE += auto_init_gnrc_netif
  # Specify the mandatory networking modules for IPv6
  USEMODULE += gnrc_ipv6_default
  # Additional networking modules that can be dropped if not needed
  USEMODULE += gnrc_icmpv6_echo
else
  USEMODULE += auto_init_lwip_netif
  USEMODULE += lwip_ipv6 lwip_ipv6_autoconfig
endif

USEMODULE += ztimer_msec

# include this for nicely formatting the returned internal value
USEMODULE += fmt

# include sha256 (used by example blockwise handler)
USEMODULE += hashes

# Comment this out to enable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
#DEVELHELP = 1

# Use different settings when compiling for one of the following (low-memory)
# boards
LOW_MEMORY_BOARDS := nucleo-f334r8

ifneq (,$(filter $(BOARD),$(LOW_MEMORY_BOARDS)))
  $(info Using low-memory configuration for microcoap_server.)
  ## low-memory tuning values
  USEMODULE += prng_minstd
endif

# Enable fileserver for boards with plenty of memory
HIGH_MEMORY_BOARDS := native native32 native64 same54-xpro mcb2388

ifneq (,$(filter $(BOARD),$(HIGH_MEMORY_BOARDS)))
  # enable separate response
  USEMODULE += event_callback
  USEMODULE += event_periodic
  USEMODULE += event_thread
  USEMODULE += event_timeout_ztimer
  USEMODULE += nanocoap_server_observe
  USEMODULE += nanocoap_server_separate

  # enable fileserver
  USEMODULE += nanocoap_fileserver
  USEMODULE += vfs_default

  # always enable auto-format for native
  ifneq (,$(filter native native32 native64,$(BOARD)))
    USEMODULE += vfs_auto_format
  endif

  USEMODULE += nanocoap_server_ws

  # async TCP is not supported on GNRC yet
  ifneq (,$(filter 1, $(LWIP_IPV4) $(LWIP_IPV6)))
    USEMODULE += nanocoap_server_tcp
  endif
endif

# if nanocaop_server_tcp is used: This app makes use of event_thread
# to run the TCP server
ifneq (,$(filter nanocoap_server_tcp,$(USEMODULE)))
  USEMODULE += event_thread
endif

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

include $(RIOTBASE)/Makefile.include

ifneq (,$(filter $(BOARD),$(LOW_MEMORY_BOARDS)))
  # lower pktbuf buffer size
  # Set GNRC_PKTBUF_SIZE via CFLAGS if not being set via Kconfig.
  ifndef CONFIG_GNRC_PKTBUF_SIZE
    CFLAGS += -DCONFIG_GNRC_PKTBUF_SIZE=1000
  endif
endif

# Set a custom channel if needed
include $(RIOTMAKE)/default-radio-settings.inc.mk
