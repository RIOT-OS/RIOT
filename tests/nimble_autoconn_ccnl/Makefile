BOARD ?= nrf52dk
include ../Makefile.tests_common

# include shell support
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps

# Include GNRC and RPL
USEMODULE += netdev_default
USEMODULE += auto_init_gnrc_netif

# Setup CCN-lite
USEPKG += ccn-lite
CFLAGS += -DUSE_LINKLAYER
CFLAGS += -DUSE_RONR
CFLAGS += -DCCNL_UAPI_H_
CFLAGS += -DUSE_SUITE_NDNTLV
CFLAGS += -DNEEDS_PREFIX_MATCHING
CFLAGS += -DNEEDS_PACKET_CRAFTING

# Setup Nimble
USEMODULE += nimble_autoconn_ndnsp

TEST_ON_CI_WHITELIST += nrf52dk

include $(RIOTBASE)/Makefile.include
