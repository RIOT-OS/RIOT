BOARD_WHITELIST = native native64 derfmega256 avr-rss2 atmega256rfr2-xpro nrf52840dongle

include ../Makefile.net_common

ifneq (,$(filter native native64,$(BOARD)))
  USEMODULE += socket_zep
  TERMFLAGS ?= -z "0.0.0.0:17755,localhost:17754"
  USEMODULE += netdev
  # somehow this breaks the test
  DISABLE_MODULE += test_utils_print_stack_usage
else
  USEMODULE += shell
  USEMODULE += shell_cmds_default
  USEMODULE += ps
  # use the default network interface for the board
  USEMODULE += netdev_default
  # shell command to send L2 packets with a simple string
  USEMODULE += gnrc_txtsnd
  # module to test rx timestamp
  USEMODULE += gnrc_netif_timestamp
endif

# gnrc is a meta module including all required, basic gnrc networking modules
USEMODULE += gnrc
# automatically initialize the network interface
USEMODULE += auto_init_gnrc_netif
# use GNRC IEEE 802.15.4 as link-layer protocol
USEMODULE += gnrc_netif_ieee802154
# the application dumps received packets to stdout
USEMODULE += gnrc_pktdump

TEST_ON_CI_WHITELIST += native native64

include $(RIOTBASE)/Makefile.include
