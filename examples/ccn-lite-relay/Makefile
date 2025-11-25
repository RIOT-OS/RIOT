APPLICATION = ccn-lite-relay

# If no BOARD is found in the environment, use this default:
BOARD ?= native

BOARD_WHITELIST := iotlab-m3 msba2 mulle native native64 nrf52dk nrf52840dk \
                   pba-d-01-kw2x samr21-xpro


# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

CFLAGS += -DUSE_LINKLAYER
CFLAGS += -DUSE_RONR
CFLAGS += -DCCNL_UAPI_H_
CFLAGS += -DUSE_SUITE_NDNTLV
CFLAGS += -DNEEDS_PREFIX_MATCHING
CFLAGS += -DNEEDS_PACKET_CRAFTING

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

USEMODULE += ps
USEMODULE += shell_cmds_default
# Include packages that pull up and auto-init the link layer.
# NOTE: 6LoWPAN will be included if IEEE802.15.4 devices are present
USEMODULE += netdev_default
USEMODULE += auto_init_gnrc_netif
# This application dumps received packets to STDIO using the pktdump module
USEMODULE += gnrc_pktdump
USEMODULE += prng_xorshift

USEPKG += ccn-lite

include $(RIOTBASE)/Makefile.include
