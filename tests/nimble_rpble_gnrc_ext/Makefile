BOARD ?= nrf52dk
include ../Makefile.tests_common

# include shell support
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps

# Enable single interface optimization.
# Remove this if more than one interface is present
USEMODULE += gnrc_netif_single
# Include GNRC and RPL
USEMODULE += auto_init_gnrc_netif
USEMODULE += gnrc_ipv6_router_default
USEMODULE += gnrc_icmpv6_echo
USEMODULE += gnrc_rpl
USEMODULE += auto_init_gnrc_rpl

# configure and use Nimble
USEMODULE += bluetil_addr
USEMODULE += nimble_rpble_ext
FEATURES_OPTIONAL += ble_phy_2mbit
FEATURES_OPTIONAL += ble_phy_coded
NIMBLE_MAX_CONN = 3

DEVELHELP = 0

TEST_ON_CI_WHITELIST += nrf52dk nrf52840dk

include $(RIOTBASE)/Makefile.include
