include ../Makefile.tests_common

# If no BOARD is found in the environment, use this default:
BOARD ?= native

ifeq (native,$(BOARD))
  PORT ?= tap0
endif

TCP_SERVER_ADDR ?= 2001:db8::affe:0001
TCP_SERVER_PORT ?= 80
TCP_TEST_CYCLES ?= 3

# Mark Boards with insufficient memory
BOARD_INSUFFICIENT_MEMORY := airfy-beacon arduino-duemilanove arduino-mega2560 \
                             arduino-uno calliope-mini chronos hifive1 mega-xplained \
                             microbit msb-430 msb-430h nrf51dk nrf51dongle nrf6310 nucleo-f031k6 \
                             nucleo-f042k6 nucleo-f303k8 nucleo-l031k6 nucleo-f030r8 \
                             nucleo-f070rb nucleo-f072rb nucleo-f302r8 nucleo-f334r8 nucleo-l053r8 \
                             saml10-xpro saml11-xpro sb-430 sb-430h stm32f0discovery telosb \
                             waspmote-pro wsn430-v1_3b wsn430-v1_4 yunjia-nrf51822 z1

# Local Address, Local Port and number of Test Cycles
CFLAGS += -DSERVER_ADDR=\"$(TCP_SERVER_ADDR)\"
CFLAGS += -DSERVER_PORT=$(TCP_SERVER_PORT)
CFLAGS += -DCYCLES=$(TCP_TEST_CYCLES)
CFLAGS += -DGNRC_NETIF_IPV6_GROUPS_NUMOF=3
CFLAGS += -DGNRC_IPV6_NIB_CONF_ARSM=1
CFLAGS += -DGNRC_IPV6_NIB_CONF_QUEUE_PKT=1

# Modules to include
USEMODULE += gnrc_netdev_default
USEMODULE += auto_init_gnrc_netif
USEMODULE += gnrc_ipv6_default
USEMODULE += gnrc_tcp

# include this for IP address manipulation
USEMODULE += shell_commands

include $(RIOTBASE)/Makefile.include
