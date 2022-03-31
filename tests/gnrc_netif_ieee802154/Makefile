BOARD_WHITELIST = native

include ../Makefile.tests_common

TERMFLAGS ?= -z "0.0.0.0:17755,localhost:17754"

USEMODULE += socket_zep
USEMODULE += auto_init_gnrc_netif
USEMODULE += netdev
USEMODULE += gnrc
USEMODULE += gnrc_netif_ieee802154
USEMODULE += gnrc_pktdump

# somehow this breaks the test
DISABLE_MODULE += test_utils_print_stack_usage

TEST_ON_CI_WHITELIST += native

include $(RIOTBASE)/Makefile.include
