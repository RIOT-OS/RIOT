include ../Makefile.tests_common

# If no BOARD is found in the environment, use this default:
BOARD ?= native
PORT ?= tap1

TCP_TARGET_ADDR ?= fe80::affe
TCP_TARGET_PORT ?= 80
TCP_TEST_CYCLES ?= 3

# Mark Boards with insufficient memory
BOARD_INSUFFICIENT_MEMORY := airfy-beacon arduino-duemilanove arduino-mega2560 \
                             arduino-uno calliope-mini chronos mega-xplained microbit \
                             msb-430 msb-430h nrf51dongle nrf6310 nucleo32-f031 \
                             nucleo32-f042 nucleo32-f303 nucleo32-l031 nucleo-f030r8 \
                             nucleo-f070rb nucleo-f072rb nucleo-f302r8 nucleo-f334r8 nucleo-l053r8 \
                             sb-430 sb-430h stm32f0discovery telosb \
                             wsn430-v1_3b wsn430-v1_4 yunjia-nrf51822 z1

# Target Address, Target Port and number of Test Cycles
CFLAGS += -DTARGET_ADDR=\"$(TCP_TARGET_ADDR)\"
CFLAGS += -DTARGET_PORT=$(TCP_TARGET_PORT)
CFLAGS += -DCYCLES=$(TCP_TEST_CYCLES)

# Modules to include
USEMODULE += gnrc_netdev_default
USEMODULE += auto_init_gnrc_netif
USEMODULE += gnrc_ipv6_default
USEMODULE += gnrc_tcp

include $(RIOTBASE)/Makefile.include
