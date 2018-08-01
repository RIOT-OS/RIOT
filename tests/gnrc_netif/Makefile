include ../Makefile.tests_common

BOARD_INSUFFICIENT_MEMORY := airfy-beacon b-l072z-lrwan1 bluepill calliope-mini \
                             cc2650-launchpad cc2650stk chronos hifive1 maple-mini \
                             microbit msb-430 msb-430h nrf51dongle nrf6310 \
                             nucleo-f030r8 nucleo-f070rb nucleo-f072rb nucleo-f103rb \
                             nucleo-f302r8 nucleo-f334r8 nucleo-l053r8 nucleo-l073rz \
                             nucleo-f031k6 nucleo-f042k6 nucleo-f303k8 \
                             nucleo-l031k6 opencm904 spark-core \
                             stm32f0discovery stm32mindev telosb \
                             wsn430-v1_3b wsn430-v1_4 yunjia-nrf51822 z1

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
CFLAGS += -DGNRC_IPV6_NIB_CONF_ARSM=0
CFLAGS += -DGNRC_IPV6_NIB_CONF_SLAAC=0
CFLAGS += -DGNRC_IPV6_NIB_CONF_NO_RTR_SOL=1
CFLAGS += -DGNRC_NETIF_NUMOF=4
CFLAGS += -DGNRC_NETIF_ADDRS_NUMOF=16
CFLAGS += -DGNRC_NETIF_GROUPS_NUMOF=8
CFLAGS += -DLOG_LEVEL=LOG_NONE
CFLAGS += -DTEST_SUITES

TEST_ON_CI_WHITELIST += all

include $(RIOTBASE)/Makefile.include

test:
	tests/01-run.py
