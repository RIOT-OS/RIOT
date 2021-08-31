BOARD ?= nrf52dk
include ../Makefile.tests_common

# include shell support
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps

# Include GNRC and RPL
USEMODULE += netdev_default
USEMODULE += auto_init_gnrc_netif
USEMODULE += gnrc_ipv6_router_default
USEMODULE += auto_init_gnrc_rpl
USEMODULE += gnrc_rpl
USEMODULE += gnrc_icmpv6_echo

# Setup Nimble
USEMODULE += nimble_statconn

TEST_ON_CI_WHITELIST += nrf52dk

include $(RIOTBASE)/Makefile.include
