DEVELHELP := 1
include ../Makefile.tests_common

USEMODULE += gnrc_ipv6_router_default
USEMODULE += gnrc_netif
USEMODULE += gnrc_pktbuf_cmd
USEMODULE += netdev_eth
USEMODULE += netdev_test
USEMODULE += od
USEMODULE += ps
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += xtimer

CFLAGS += -DCONFIG_GNRC_PKTBUF_SIZE=512
CFLAGS += -DTEST_SUITES

include $(RIOTBASE)/Makefile.include
