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

CFLAGS += -DTEST_SUITES

include $(RIOTBASE)/Makefile.include

# Set GNRC_PKTBUF_SIZE via CFLAGS if not being set via Kconfig.
ifndef CONFIG_GNRC_PKTBUF_SIZE
  CFLAGS += -DCONFIG_GNRC_PKTBUF_SIZE=512
endif
