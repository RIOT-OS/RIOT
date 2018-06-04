# name of your application
APPLICATION = gnrc_tftp_example

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

BOARD_INSUFFICIENT_MEMORY := airfy-beacon b-l072z-lrwan1 bluepill calliope-mini chronos \
                             hifive1 microbit msb-430 msb-430h nrf51dongle nrf6310 nucleo-f031k6 \
                             nucleo-f042k6 nucleo-f303k8 nucleo-l031k6 nucleo-f030r8 \
                             nucleo-f070rb nucleo-f072rb nucleo-f103rb nucleo-f302r8 nucleo-f334r8 \
                             nucleo-l053r8 spark-core stm32f0discovery stm32mindev \
                             telosb wsn430-v1_3b wsn430-v1_4 yunjia-nrf51822 z1

# Include packages that pull up and auto-init the link layer.
# NOTE: 6LoWPAN will be included if IEEE802.15.4 devices are present
USEMODULE += gnrc_netdev_default
USEMODULE += auto_init_gnrc_netif
# Specify the mandatory networking modules for IPv6 and UDP
USEMODULE += gnrc_ipv6_router_default
USEMODULE += gnrc_udp
# Add a routing protocol
USEMODULE += gnrc_rpl
# This application dumps received packets to STDIO using the pktdump module
USEMODULE += gnrc_pktdump
# Additional networking modules that can be dropped if not needed
USEMODULE += gnrc_icmpv6_echo
USEMODULE += gnrc_tftp
# Add also the shell, some shell commands
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

include $(RIOTBASE)/Makefile.include
