include ../Makefile.tests_common

BOARD_INSUFFICIENT_MEMORY := chronos msb-430 msb-430h nucleo-f031k6 nucleo-f042k6 \
                             nucleo-f303k8 nucleo-l031k6 nucleo-f030r8 \
                             nucleo-l053r8 stm32f0discovery telosb z1

# Include packages that pull up and auto-init the link layer.
# NOTE: 6LoWPAN will be included if IEEE802.15.4 devices are present
USEMODULE += gnrc_netdev_default
USEMODULE += auto_init_gnrc_netif
# Specify the mandatory networking modules for IPv6 and UDP
USEMODULE += gnrc_ipv6_default
USEMODULE += gnrc_udp
# Additional networking modules that can be dropped if not needed
USEMODULE += gnrc_icmpv6_echo

#
USEMODULE += gnrc_sock_udp

USEPKG += microcoap

# include this for printing IP addresses
USEMODULE += shell_commands

# Use different settings when compiling for one of the following (low-memory)
# boards
LOW_MEMORY_BOARDS := nucleo-f334r8

ifneq (,$(filter $(BOARD),$(LOW_MEMORY_BOARDS)))
  $(info Using low-memory configuration for microcoap_server.)
  ## low-memory tuning values
  # lower pktbuf buffer size
  CFLAGS += -DGNRC_PKTBUF_SIZE=1000
  USEMODULE += prng_minstd
endif

include $(RIOTBASE)/Makefile.include
