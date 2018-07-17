include ../Makefile.tests_common

BOARD_INSUFFICIENT_MEMORY := chronos msb-430 msb-430h nucleo-f031k6 \
                             nucleo-f042k6 nucleo-l031k6 nucleo-f030r8 \
                             nucleo-f303k8 nucleo-f334r8 nucleo-l053r8 \
                             stm32f0discovery telosb wsn430-v1_3b wsn430-v1_4 z1

USEMODULE += sntp
USEMODULE += gnrc_sock_udp
USEMODULE += gnrc_ipv6_default
USEMODULE += auto_init_gnrc_netif
USEMODULE += gnrc_netdev_default
USEMODULE += shell
USEMODULE += shell_commands

include $(RIOTBASE)/Makefile.include
