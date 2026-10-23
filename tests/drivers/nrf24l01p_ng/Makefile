DEBUG ?= 0

include ../Makefile.drivers_common

USEMODULE += shell_cmds_default
USEMODULE += ps
USEMODULE += gnrc
USEMODULE += auto_init_gnrc_netif
USEMODULE += nrf24l01p_ng
USEMODULE += nrf24l01p_ng_diagnostics
USEMODULE += netstats_l2
USEMODULE += gnrc_txtsnd

CFLAGS += -DCONFIG_ASSERT_VERBOSE=1
include $(RIOTBASE)/Makefile.include
