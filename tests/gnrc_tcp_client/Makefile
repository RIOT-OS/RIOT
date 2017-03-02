# name of your application
APPLICATION = gnrc_tcp_client

# If no BOARD is found in the environment, use this default:
BOARD ?= native
PORT ?= tap1

TCP_TARGET_ADDR ?= fe80::5c38:e9ff:fe76:6195
TCP_TARGET_PORT ?= 80
TCP_TEST_CYCLES ?= 10

# Mark Boards with insufficient memory
BOARD_INSUFFICIENT_MEMORY := airfy-beacon arduino-duemilanove arduino-mega2560\
                             arduino-uno calliope-mini chronos microbit sb-430\
                             sb-430h nrf51dongle nrf6310 nucleo-f030\
                             nucleo32-f042 nucleo-f070 nucleo-f072 nucleo32-f303\
                             nucleo-f334 pca10000 pca10005 stm32f0discovery\
                             telosb weio wsn430-v1_3b wsn430-v1_4\
                             yunjia-nrf51822 z1 msb-430 msb-430h nucleo32-f031\
                             nucleo-l053

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

# Target Address, Target Port and number of Test Cycles
CFLAGS += -DTARGET_ADDR=\"$(TCP_TARGET_ADDR)\"
CFLAGS += -DTARGET_PORT=$(TCP_TARGET_PORT)
CFLAGS += -DCYCLES=$(TCP_TEST_CYCLES)

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
#CFLAGS += -DDEVELHELP

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

# Modules to include
USEMODULE += gnrc_netdev_default
USEMODULE += auto_init_gnrc_netif
USEMODULE += gnrc_ipv6_default
USEMODULE += gnrc_tcp

include $(RIOTBASE)/Makefile.include
