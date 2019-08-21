DEVELHELP := 1
include ../Makefile.tests_common

BOARD_INSUFFICIENT_MEMORY := arduino-duemilanove arduino-leonardo \
                             arduino-mega2560 arduino-nano arduino-uno chronos \
                             i-nucleo-lrwan1 msb-430 msb-430h nucleo-f030r8 \
                             nucleo-f031k6 nucleo-f042k6 nucleo-l031k6 \
                             nucleo-l053r8 stm32f0discovery stm32l0538-disco telosb \
                             waspmote-pro wsn430-v1_3b wsn430-v1_4 z1

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

CFLAGS += -DGNRC_PKTBUF_SIZE=512
CFLAGS += -DTEST_SUITES

include $(RIOTBASE)/Makefile.include
