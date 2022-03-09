DEBUG ?= 0

include ../Makefile.tests_common

USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps
USEMODULE += gnrc
USEMODULE += auto_init_gnrc_netif
USEMODULE += nrf24l01p_ng
USEMODULE += nrf24l01p_ng_diagnostics
USEMODULE += netstats_l2
USEMODULE += gnrc_txtsnd

CFLAGS += -DDEBUG_ASSERT_VERBOSE=1
include $(RIOTBASE)/Makefile.include
