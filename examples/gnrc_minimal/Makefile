# name of your application
APPLICATION = gnrc_minimal

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

BOARD_INSUFFICIENT_MEMORY := chronos msb-430 msb-430h

# Include packages that pull up and auto-init the link layer.
# NOTE: 6LoWPAN will be included if IEEE802.15.4 devices are present
USEMODULE += gnrc_netif_default
USEMODULE += auto_init_gnrc_netif
# Specify the minimum networking modules for IPv6
USEMODULE += gnrc_ipv6
USEMODULE += gnrc_ndp
# Additional networking modules that can be dropped if not needed
USEMODULE += gnrc_icmpv6_echo
# Use minimal standard PRNG
USEMODULE += prng_minstd

CFLAGS += -DGNRC_IPV6_NETIF_ADDR_NUMOF=4
# Reduce sizes for GNRC's packet buffer and neighbor cache
# (for native we need a bigger buffer, because the tap driver temporarily
# allocates ~1500 bytes)
ifneq (,$(filter native,$(BOARD)))
    CFLAGS += -DGNRC_PKTBUF_SIZE=2048 -DGNRC_IPV6_NC_SIZE=1
else
    CFLAGS += -DGNRC_PKTBUF_SIZE=512 -DGNRC_IPV6_NC_SIZE=0
endif

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

include $(RIOTBASE)/Makefile.include
