USEMODULE += shell_cmd_gnrc_icmpv6_echo
USEMODULE += shell_cmd_gnrc_ipv6_nib
USEMODULE += shell_cmd_gnrc_netif
USEMODULE += shell_cmd_gnrc_pktbuf
USEMODULE += shell_cmd_gnrc_txtsnd

USEMODULE += l2filter_whitelist
USEMODULE += fmt
USEMODULE += gnrc_netif
USEMODULE += gnrc_sixlowpan_border_router_default
USEMODULE += netdev_eth
USEMODULE += netdev_ieee802154
USEMODULE += netdev_test
USEMODULE += netstats_l2
USEMODULE += netstats_ipv6
USEMODULE += od
USEMODULE += shell_cmd_sys
USEMODULE += sock_dns_mock

include $(CURDIR)/../Makefile.riotctrl_shell_common

CFLAGS += -D"TESTING_MOCK_SHELL_CMD_NETIF_LORA"
# use ::1 for example.org
CFLAGS += -D"SOCK_DNS_MOCK_ADDR_IPV6={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1}"
