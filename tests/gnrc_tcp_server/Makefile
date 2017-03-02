# name of your application
APPLICATION = gnrc_tcp_server

# If no BOARD is found in the environment, use this default:
BOARD ?= native
PORT ?= tap0

TCP_LOCAL_PORT ?= 80

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

# Specify local Port to open
CFLAGS += -DLOCAL_PORT=$(TCP_LOCAL_PORT)

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

# include this for printing IP addresses
USEMODULE += shell_commands

include $(RIOTBASE)/Makefile.include
