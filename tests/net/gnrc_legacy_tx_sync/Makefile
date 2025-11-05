include ../Makefile.net_common

USEMODULE += netdev_default
USEMODULE += auto_init_gnrc_netif
USEMODULE += gnrc_tx_sync
USEMODULE += gnrc_netif_pktq
USEMODULE += nrfmin
USEMODULE += ztimer_sec

FEATURES_REQUIRED += cpu_nrf52

include $(RIOTBASE)/Makefile.include

TEST_ON_CI_WHITELIST += nrf52840dk

CFLAGS += -DDEBUG_ASSERT_VERBOSE=1
