include ../Makefile.tests_common

BOARD_INSUFFICIENT_MEMORY := msb-430 msb-430h nucleo32-f031 nucleo32-f042 \
                             nucleo32-l031 nucleo-l053 stm32f0discovery telosb \
                             weio z1

USEPKG += winc1500

USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps

# Comment out when you don't need GNRC.
USEMODULE += auto_init_gnrc_netif
USEMODULE += gnrc_netdev_default
USEMODULE += gnrc_ipv6_router_default
USEMODULE += gnrc_icmpv6_echo

include $(RIOTBASE)/Makefile.include
