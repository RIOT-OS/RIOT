include ../Makefile.tests_common

USEMODULE += embunit
USEMODULE += gnrc_netif
USEMODULE += gnrc_pktdump
USEMODULE += gnrc_sixlowpan
USEMODULE += gnrc_sixlowpan_iphc
USEMODULE += gnrc_ipv6
USEMODULE += netdev_eth
USEMODULE += netdev_ieee802154
USEMODULE += netdev_test
USEMODULE += od

# deactivate automatically emitted packets from IPv6 neighbor discovery
CFLAGS += -DCONFIG_GNRC_IPV6_NIB_ARSM=0
CFLAGS += -DCONFIG_GNRC_IPV6_NIB_SLAAC=0
CFLAGS += -DCONFIG_GNRC_IPV6_NIB_NO_RTR_SOL=1
CFLAGS += -DGNRC_NETIF_ADDRS_NUMOF=16
CFLAGS += -DGNRC_NETIF_GROUPS_NUMOF=8
CFLAGS += -DLOG_LEVEL=LOG_NONE
CFLAGS += -DTEST_SUITES

include $(RIOTBASE)/Makefile.include
