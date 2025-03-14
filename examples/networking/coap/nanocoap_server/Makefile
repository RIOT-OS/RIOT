# name of your application
APPLICATION = nanocoap_server

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../../../..

# Include packages that pull up and auto-init the link layer.
# NOTE: 6LoWPAN will be included if IEEE802.15.4 devices are present
USEMODULE += netdev_default
USEMODULE += auto_init_gnrc_netif
# Specify the mandatory networking modules for IPv6 and UDP
USEMODULE += gnrc_ipv6_default
USEMODULE += sock_udp
# Additional networking modules that can be dropped if not needed
USEMODULE += gnrc_icmpv6_echo

USEMODULE += nanocoap_sock
USEMODULE += nanocoap_resources

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
