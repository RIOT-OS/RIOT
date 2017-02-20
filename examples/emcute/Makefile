# name of your application
APPLICATION = emcute

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

BOARD_INSUFFICIENT_MEMORY := arduino-duemilanove arduino-mega2560 arduino-uno \
                             chronos msb-430 msb-430h nucleo-f030 nucleo-l053 \
                             nucleo-f070 nucleo-f072 nucleo-f334 nucleo32-f031 \
                             nucleo32-f303 nucleo32-f042 stm32f0discovery telosb \
                             waspmote-pro weio wsn430-v1_3b wsn430-v1_4 z1

# Include packages that pull up and auto-init the link layer.
# NOTE: 6LoWPAN will be included if IEEE802.15.4 devices are present
USEMODULE += gnrc_netdev_default
USEMODULE += auto_init_gnrc_netif
# Specify the mandatory networking modules for IPv6 and UDP
USEMODULE += gnrc_sock_udp
USEMODULE += gnrc_ipv6_default
# Include MQTT-SN
USEMODULE += emcute
# Add also the shell, some shell commands
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps
# For testing we also include the ping6 command and some stats
USEMODULE += gnrc_icmpv6_echo

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
CFLAGS += -DDEVELHELP

# Comment this out to join RPL DODAGs even if DIOs do not contain
# DODAG Configuration Options (see the doc for more info)
# CFLAGS += -DGNRC_RPL_DODAG_CONF_OPTIONAL_ON_JOIN

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

include $(RIOTBASE)/Makefile.include
