include ../Makefile.tests_common

USEMODULE += gnrc_tx_sync
USEMODULE += netdev_test
USEMODULE += sock_udp
USEMODULE += xtimer
USEMODULE += auto_init_gnrc_netif
USEMODULE += gnrc_ipv6_default

include $(RIOTBASE)/Makefile.include

# This is needed for support of NETDEV_TYPE_TEST
CFLAGS += -DTEST_SUITES
