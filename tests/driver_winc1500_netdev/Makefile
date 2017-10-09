APPLICATION = driver_winc1500_netdev
include ../Makefile.tests_common

BOARD_INSUFFICIENT_MEMORY := msb-430 msb-430h nucleo32-f031 nucleo32-f042 \
                             nucleo32-l031 nucleo-l053 stm32f0discovery telosb \
                             weio z1

USEMODULE += winc1500
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps

# Comment out when you don't need GNRC.
USEMODULE += gnrc_netdev
USEMODULE += gnrc_netdev_default
USEMODULE += auto_init_gnrc_netif
USEMODULE += gnrc_ipv6_router_default
USEMODULE += gnrc_icmpv6_echo

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
CFLAGS += -DDEVELHELP -g3 -ggdb3 -O0 -Og

# Debugging flags. Uncomment to enable debugging mode
CFLAGS += -DLOG_LEVEL=LOG_DEBUG

include $(RIOTBASE)/Makefile.include
