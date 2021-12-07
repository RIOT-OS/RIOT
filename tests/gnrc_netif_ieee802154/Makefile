BOARD_WHITELIST = native

include ../Makefile.tests_common

TERMFLAGS ?= -z "0.0.0.0:17755,localhost:17754"

USEMODULE += socket_zep
USEMODULE += auto_init_gnrc_netif
USEMODULE += netdev
USEMODULE += gnrc
USEMODULE += gnrc_netif_ieee802154
USEMODULE += gnrc_pktdump

TEST_ON_CI_WHITELIST += native

include $(RIOTBASE)/Makefile.include
