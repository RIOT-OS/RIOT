BOARD ?= nrf52dk
include ../Makefile.tests_common

# include shell support
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps

# Include GNRC and RPL
USEMODULE += gnrc_netif_single
USEMODULE += auto_init_gnrc_netif
USEMODULE += gnrc_ipv6_router_default
USEMODULE += gnrc_icmpv6_echo
USEMODULE += gnrc_rpl
USEMODULE += auto_init_gnrc_rpl

# configure and use Nimble
USEMODULE += bluetil_addr
USEMODULE += nimble_rpble
NIMBLE_MAX_CONN := 3

DEVELHELP = 0

TEST_ON_CI_WHITELIST += nrf52dk

include $(RIOTBASE)/Makefile.include
