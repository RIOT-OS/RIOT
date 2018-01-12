# name of your application
APPLICATION = gnrc_xbee_border_router

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

FEATURES_REQUIRED += periph_uart periph_gpio ethernet

BOARD_INSUFFICIENT_MEMORY := airfy-beacon msb-430 msb-430h pca10000 pca10005 \
                             nrf51dongle nrf6310  nucleo-f334 spark-core \
                             stm32f0discovery telosb weio wsn430-v1_3b \
                             wsn430-v1_4 yunjia-nrf51822 z1

GNRC_NETIF_NUMOF := 2
INCLUDES += -I$(CURDIR)

# UART port number
XBEE_PARAM_UART = 0
CFLAGS += -DXBEE_PARAM_UART=$(XBEE_PARAM_UART)

# UART baud rate
XBEE_PARAM_BR = 9600U
CFLAGS += -DXBEE_PARAM_BR=$(XBEE_PARAM_BR)

# Uncomment this for debugging
CFLAGS += -g

# Uncomment this to emulate wireless network in which we cannot communicate to
# some nodes directly.
# CFLAGS += "-DXBEE_DENIED_ADDRESSES={ 0x02, 0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xde, 0x02, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11 }"

# Include packages that pull up and auto-init the link layer.
# NOTE: 6LoWPAN will be included if IEEE802.15.4 devices are present
USEMODULE += gnrc_netdev_default
USEMODULE += auto_init_gnrc_netif
# Specify the mandatory networking modules for 6LoWPAN border router
USEMODULE += gnrc_sixlowpan_border_router_default
# Add routing protocol
USEMODULE += gnrc_rpl
# Additional networking modules that can be dropped if not needed
USEMODULE += gnrc_icmpv6_echo
USEMODULE += gnrc_icmpv6_error
USEMODULE += gnrc_ipv6_ext
# XBee network stack
USEMODULE += xbee
USEMODULE += gnrc_netdev2
USEMODULE += gnrc_pktdump
# Add also the shell, some shell commands
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
CFLAGS += -DDEVELHELP

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

include $(RIOTBASE)/Makefile.include
