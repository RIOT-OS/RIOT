DEBUG ?= 0

include ../Makefile.tests_common

USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps
USEMODULE += gnrc
USEMODULE += auto_init_gnrc_netif
USEMODULE += nrf24l01p
USEMODULE += nrf24l01p_advanced
USEMODULE += netstats_l2
USEMODULE += gnrc_ipv6
USEMODULE += netstats_ipv6
USEMODULE += gnrc_ipv6_router_default
USEMODULE += gnrc_icmpv6
USEMODULE += gnrc_icmpv6_error
USEMODULE += gnrc_icmpv6_echo
USEMODULE += gnrc_txtsnd
USEMODULE += gnrc_pktdump

CFLAGS += -DDEBUG_ASSERT_VERBOSE=1
include $(RIOTBASE)/Makefile.include
