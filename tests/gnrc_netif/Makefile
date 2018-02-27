include ../Makefile.tests_common

BOARD_INSUFFICIENT_MEMORY := airfy-beacon b-l072z-lrwan1 bluepill calliope-mini \
                             cc2650-launchpad cc2650stk chronos maple-mini \
                             microbit msb-430 msb-430h nrf51dongle nrf6310 \
                             nucleo-f030r8 nucleo-f070rb nucleo-f072 nucleo-f103 \
                             nucleo-f302 nucleo-f334 nucleo-l053 nucleo-l073 \
                             nucleo32-f031 nucleo32-f042 nucleo32-f303 \
                             nucleo32-l031 opencm904 spark-core \
                             stm32f0discovery telosb wsn430-v1_3b wsn430-v1_4 \
                             yunjia-nrf51822 z1

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

CFLAGS += -DGNRC_NETIF_NUMOF=4
CFLAGS += -DGNRC_NETIF_ADDRS_NUMOF=16
CFLAGS += -DGNRC_NETIF_GROUPS_NUMOF=8
CFLAGS += -DLOG_LEVEL=LOG_NONE
CFLAGS += -DTEST_SUITES

include $(RIOTBASE)/Makefile.include

test:
	tests/01-run.py
