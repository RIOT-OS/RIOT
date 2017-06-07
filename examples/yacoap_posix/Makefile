# name of your application
APPLICATION = yacoap_posix

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

BOARD_INSUFFICIENT_MEMORY := airfy-beacon chronos msb-430 msb-430h nrf51dongle \
                          nrf6310 pca10000 pca10005 spark-core \
                          stm32f0discovery telosb weio wsn430-v1_3b wsn430-v1_4 \
                          yunjia-nrf51822 z1 nucleo-f072

# Include packages that pull up and auto-init the link layer.
# NOTE: 6LoWPAN will be included if IEEE802.15.4 devices are present
USEMODULE += gnrc_netdev_default
USEMODULE += auto_init_gnrc_netif
# Specify the mandatory networking modules for IPv6 and UDP
USEMODULE += gnrc_ipv6_default
USEMODULE += gnrc_udp
USEMODULE += gnrc_conn_udp
USEMODULE += posix_sockets
USEMODULE += gnrc_icmpv6_echo
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps
# include YaCoAP package
USEPKG += yacoap

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
$(info Using low-memory configuration for microcoap_server.)
## low-memory tuning values
# lower pktbuf buffer size
CFLAGS += -DGNRC_PKTBUF_SIZE=1000
# disable fib, rpl
DISABLE_MODULE += fib gnrc_rpl
USEMODULE += prng_minstd
endif

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

include $(RIOTBASE)/Makefile.include
