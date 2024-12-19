# name of your application
APPLICATION = nanocoap_server

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

NETWORK_STACK ?= gnrc

# Include packages that pull up and auto-init the link layer.
# NOTE: 6LoWPAN will be included if IEEE802.15.4 devices are present
USEMODULE += netdev_default
USEMODULE += sock_udp
USEMODULE += ipv6_addr

ifeq ($(NETWORK_STACK),gnrc)
  USEMODULE += auto_init_gnrc_netif
  # Specify the mandatory networking modules for IPv6
  USEMODULE += gnrc_ipv6_default
  # Additional networking modules that can be dropped if not needed
  USEMODULE += gnrc_icmpv6_echo
endif
ifeq ($(NETWORK_STACK),lwip)
  USEMODULE += auto_init_lwip_netif
  USEMODULE += lwip_ipv6 lwip_ipv6_autoconfig
endif

USEMODULE += xtimer

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

# Enable fileserver and TCP for boards with plenty of memory
HIGH_MEMORY_BOARDS := native native64 same54-xpro mcb2388

ifneq (,$(filter $(BOARD),$(HIGH_MEMORY_BOARDS)))
  # enable separate response
  USEMODULE += nanocoap_server_separate
  USEMODULE += event_callback
  USEMODULE += event_thread
  USEMODULE += event_timeout_ztimer

  # enable fileserver
  USEMODULE += nanocoap_fileserver
  USEMODULE += vfs_default

  # always enable auto-format for native
  ifneq (,$(filter native native64,$(BOARD)))
    USEMODULE += vfs_auto_format
  endif

  # async TCP is not supported on GNRC yet
  ifeq ($(NETWORK_STACK),lwip)
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
