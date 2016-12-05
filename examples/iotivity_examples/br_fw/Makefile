# name of your application
APPLICATION = ocf_br_fw

CFLAGS += -DOC_SERVER
CFLAGS += -DOC_CLIENT

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../../..

BOARD_INSUFFICIENT_MEMORY := airfy-beacon chronos msb-430 msb-430h nrf51dongle \
                          nrf6310 pca10000 pca10005 spark-core \
                          stm32f0discovery telosb weio wsn430-v1_3b wsn430-v1_4 \
                          yunjia-nrf51822 z1 nucleo-f072

# use ethos (ethernet over serial) for network communication and stdio over
# UART, but not on native, as native has a tap interface towards the host.
ifeq (,$(filter native,$(BOARD)))
GNRC_NETIF_NUMOF := 2
USEMODULE += ethos gnrc_netdev2

# ethos baudrate can be configured from make command
ETHOS_BAUDRATE ?= 115200
CFLAGS += '-DETHOS_UART=UART_DEV(0)' -DETHOS_BAUDRATE=$(ETHOS_BAUDRATE) -DUSE_ETHOS_FOR_STDIO
FEATURES_REQUIRED += periph_uart
endif

# Include packages that pull up and auto-init the link layer.
# NOTE: 6LoWPAN will be included if IEEE802.15.4 devices are present
USEMODULE += gnrc_netdev_default
USEMODULE += auto_init_gnrc_netif
# Specify the mandatory networking modules for 6LoWPAN border router
USEMODULE += gnrc_sixlowpan_border_router_default
# Add forwarding table
USEMODULE += fib
# Additional networking modules that can be dropped if not needed
USEMODULE += gnrc_icmpv6_echo
# Add also the shell, some shell commands
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps
#USEMODULE += gnrc_pktdump
# include UHCP client
USEMODULE += gnrc_uhcpc
#UDP
USEMODULE += gnrc_udp
USEMODULE += gnrc_conn_udp
#rpl
# Add a routing protocol
USEMODULE += gnrc_rpl
USEMODULE += auto_init_gnrc_rpl
USEMODULE += gnrc_ipv6_router_default
USEMODULE += gnrc_ndp_router
#Other modules
USEMODULE += pthread
USEMODULE += shell
USEMODULE += shell_commands

SECURE ?= 0
export SECURE


# Set a custom 802.15.4 channel if needed
DEFAULT_CHANNEL ?= 26
CFLAGS += -DDEFAULT_CHANNEL=$(DEFAULT_CHANNEL)

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
#CFLAGS += -DDEVELHELP

# Use different settings when compiling for one of the following (low-memory)
# boards
LOW_MEMORY_BOARDS := nucleo-f334

ifneq (,$(filter $(BOARD),$(LOW_MEMORY_BOARDS)))
$(info Using low-memory configuration for iotivity_server.)
## low-memory tuning values
# lower pktbuf buffer size
CFLAGS += -DGNRC_PKTBUF_SIZE=1000
# disable fib, rpl
DISABLE_MODULE += fib gnrc_rpl
USEMODULE += prng_minstd
endif

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

TAP ?= tap0
IPV6_PREFIX ?= 2001:db8::/64
IPV6_MCAST_OIC ?= ff03::158

include $(RIOTBASE)/Makefile.include
.PHONY: host-tools

term: host-tools
	$(AD) sudo ./start_network_mcast.sh $(PORT) $(TAP) $(IPV6_PREFIX) $(IPV6_MCAST_OIC)

host-tools:
	$(AD)env -u CC -u CFLAGS make -C $(RIOTBASE)/dist/tools
