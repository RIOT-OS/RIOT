USEMODULE += shell_cmd_lwip_netif

DISABLE_MODULE += auto_init_lwip

USEMODULE += lwip
USEMODULE += lwip_contrib
USEMODULE += lwip_ethernet
USEMODULE += lwip_ipv6
USEMODULE += lwip_sixlowpan
USEMODULE += netdev_eth
USEMODULE += netdev_ieee802154
USEMODULE += netdev_test
USEMODULE += netif
USEMODULE += od
USEMODULE += shell_cmd_sys

include $(CURDIR)/../Makefile.riotctrl_shell_common

CFLAGS += -DETHARP_SUPPORT_STATIC_ENTRIES=1
