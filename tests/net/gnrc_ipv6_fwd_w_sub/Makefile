DEVELHELP := 1
include ../Makefile.net_common

USEMODULE += gnrc_ipv6_router_default
USEMODULE += gnrc_netif
USEMODULE += shell_cmd_gnrc_pktbuf
USEMODULE += netdev_eth
USEMODULE += netdev_test
USEMODULE += od
USEMODULE += ps
USEMODULE += shell
USEMODULE += shell_cmds_default
USEMODULE += xtimer

CFLAGS += -DTEST_SUITES

# microbit qemu failing currently
TEST_ON_CI_BLACKLIST += microbit

include $(RIOTBASE)/Makefile.include

# Set GNRC_PKTBUF_SIZE via CFLAGS if not being set via Kconfig.
ifndef CONFIG_GNRC_PKTBUF_SIZE
  CFLAGS += -DCONFIG_GNRC_PKTBUF_SIZE=512
endif
