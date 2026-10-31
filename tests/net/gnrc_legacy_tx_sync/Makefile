include ../Makefile.net_common

USEMODULE += netdev_default
USEMODULE += auto_init_gnrc_netif
USEMODULE += gnrc_tx_sync
USEMODULE += gnrc_netif_pktq
USEMODULE += nrfmin
USEMODULE += ztimer_sec

include $(RIOTBASE)/Makefile.include

CFLAGS += -DCONFIG_ASSERT_VERBOSE=1
