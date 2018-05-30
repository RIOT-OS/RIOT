include ../Makefile.tests_common

# If no BOARD is found in the environment, use this default:
BOARD ?= native
PORT ?= tap0

TCP_LOCAL_ADDR ?= fe80::affe
TCP_LOCAL_PORT ?= 80
TCP_TEST_CYCLES ?= 3

# Mark Boards with insufficient memory
BOARD_INSUFFICIENT_MEMORY := airfy-beacon arduino-duemilanove arduino-mega2560 \
                             arduino-uno calliope-mini chronos hifive1 mega-xplained \
                             microbit msb-430 msb-430h nrf51dongle nrf6310 nucleo-f031k6 \
                             nucleo-f042k6 nucleo-f303k8 nucleo-l031k6 nucleo-f030r8 \
                             nucleo-f070rb nucleo-f072rb nucleo-f302r8 nucleo-f334r8 nucleo-l053r8 \
                             sb-430 sb-430h stm32f0discovery telosb \
                             wsn430-v1_3b wsn430-v1_4 yunjia-nrf51822 z1

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

# Local Address, Local Port and number of Test Cycles
CFLAGS += -DLOCAL_ADDR=\"$(TCP_LOCAL_ADDR)\"
CFLAGS += -DLOCAL_PORT=$(TCP_LOCAL_PORT)
CFLAGS += -DCYCLES=$(TCP_TEST_CYCLES)
CFLAGS += -DGNRC_NETIF_IPV6_GROUPS_NUMOF=3

# Modules to include
USEMODULE += gnrc_netdev_default
USEMODULE += auto_init_gnrc_netif
USEMODULE += gnrc_ipv6_default
USEMODULE += gnrc_tcp

# include this for IP address manipulation
USEMODULE += shell_commands

include $(RIOTBASE)/Makefile.include
