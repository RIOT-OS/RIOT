APPLICATION = driver_netdev_eth
include ../Makefile.tests_common

BOARD_WHITELIST := native

USEMODULE += dev_eth_tap
USEMODULE += ng_netbase
USEMODULE += ng_nomac
USEMODULE += ng_pktdump
USEMODULE += ng_netdev_eth
USEMODULE += auto_init_ng_netif
USEMODULE += shell
USEMODULE += shell_commands

include $(RIOTBASE)/Makefile.include
