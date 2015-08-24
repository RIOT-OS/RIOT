APPLICATION = driver_netdev_eth
include ../Makefile.tests_common

BOARD_WHITELIST := native

USEMODULE += dev_eth_tap
USEMODULE += gnrc
USEMODULE += gnrc_nomac
USEMODULE += gnrc_pktdump
USEMODULE += gnrc_netdev_eth
USEMODULE += auto_init_ng_netif
USEMODULE += shell
USEMODULE += shell_commands

include $(RIOTBASE)/Makefile.include
