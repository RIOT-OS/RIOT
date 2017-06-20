# name of your application
APPLICATION = sntp
include ../Makefile.tests_common

BOARD_INSUFFICIENT_MEMORY := chronos msb-430 msb-430h nucleo32-f031 \
                             nucleo32-f042 nucleo32-l031 nucleo-f030 \
                             nucleo-f334 nucleo-l053 stm32f0discovery \
                             telosb weio z1

USEMODULE += sntp
USEMODULE += gnrc_sock_udp
USEMODULE += gnrc_ipv6_default
USEMODULE += auto_init_gnrc_netif
USEMODULE += gnrc_netdev_default
USEMODULE += shell
USEMODULE += shell_commands

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
CFLAGS += -DDEVELHELP

include $(RIOTBASE)/Makefile.include
