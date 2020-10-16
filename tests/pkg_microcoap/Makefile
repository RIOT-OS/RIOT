include ../Makefile.tests_common

# Include packages that pull up and auto-init the link layer.
# NOTE: 6LoWPAN will be included if IEEE802.15.4 devices are present
USEMODULE += gnrc_netdev_default
USEMODULE += auto_init_gnrc_netif
# Specify the mandatory networking modules for IPv6
USEMODULE += gnrc_ipv6_default
# Additional networking modules that can be dropped if not needed
USEMODULE += gnrc_icmpv6_echo

USEMODULE += sock_udp

USEPKG += microcoap

# include this for printing IP addresses
USEMODULE += shell_commands

# Use different settings when compiling for one of the following (low-memory)
# boards
LOW_MEMORY_BOARDS := nucleo-f334r8

ifneq (,$(filter $(BOARD),$(LOW_MEMORY_BOARDS)))
  $(info Using low-memory configuration for microcoap_server.)
  ## low-memory tuning values
  USEMODULE += prng_minstd
endif

include $(RIOTBASE)/Makefile.include

ifneq (,$(filter $(BOARD),$(LOW_MEMORY_BOARDS)))
  # lower pktbuf buffer size
  # Set GNRC_PKTBUF_SIZE via CFLAGS if not being set via Kconfig.
  ifndef CONFIG_GNRC_PKTBUF_SIZE
    CFLAGS += -DCONFIG_GNRC_PKTBUF_SIZE=1000
  endif
endif
