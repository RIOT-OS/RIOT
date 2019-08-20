BOARD ?= msba2
include ../Makefile.tests_common

DEVICE ?= cc1100 # The MSB-A2 uses the CC1100. New boards use CC1101

BOARD_INSUFFICIENT_MEMORY += arduino-duemilanove
BOARD_INSUFFICIENT_MEMORY += arduino-leonardo
BOARD_INSUFFICIENT_MEMORY += arduino-mega2560
BOARD_INSUFFICIENT_MEMORY += arduino-nano
BOARD_INSUFFICIENT_MEMORY += arduino-uno
BOARD_INSUFFICIENT_MEMORY += blackpill
BOARD_INSUFFICIENT_MEMORY += bluepill
BOARD_INSUFFICIENT_MEMORY += i-nucleo-lrwan1
BOARD_INSUFFICIENT_MEMORY += mega-xplained
BOARD_INSUFFICIENT_MEMORY += nucleo-f031k6
BOARD_INSUFFICIENT_MEMORY += nucleo-f042k6
BOARD_INSUFFICIENT_MEMORY += nucleo-f072rb
BOARD_INSUFFICIENT_MEMORY += nucleo-f302r8
BOARD_INSUFFICIENT_MEMORY += nucleo-f303k8
BOARD_INSUFFICIENT_MEMORY += nucleo-f334r8
BOARD_INSUFFICIENT_MEMORY += nucleo-l031k6
BOARD_INSUFFICIENT_MEMORY += nucleo-l053r8
BOARD_INSUFFICIENT_MEMORY += saml10-xpro
BOARD_INSUFFICIENT_MEMORY += saml11-xpro
BOARD_INSUFFICIENT_MEMORY += stm32f0discovery
BOARD_INSUFFICIENT_MEMORY += stm32l0538-disco
BOARD_INSUFFICIENT_MEMORY += waspmote-pro

# stdlib.h for msp430 does not provide EXIT_FAILURE and EXIT_SUCCESS
BOARD_BLACKLIST += msb-430 msb-430h telosb wsn430-v1_3b wsn430-v1_4 z1

# This test will rely on a human interacting with the shell, so we better add
# the shell and some commands
USEMODULE += shell
USEMODULE += shell_commands
# Add the device driver
USEMODULE += $(DEVICE)
# Add a network stack and auto init of the network devices
USEMODULE += gnrc
USEMODULE += auto_init_gnrc_netif
USEMODULE += gnrc_ipv6_router_default
# checking current state of network threads can be useful for testing/debuggig
USEMODULE += ps
# txtsnd can be useful to test lower layers
USEMODULE += gnrc_txtsnd
USEMODULE += auto_init_gnrc_netif
# Activate ICMPv6 error messages
USEMODULE += gnrc_icmpv6_error
# This application dumps received packets to STDIO using the pktdump module
USEMODULE += gnrc_pktdump
# Additional networking modules that can be dropped if not needed
USEMODULE += gnrc_icmpv6_echo
# Some statistics could also be helpful
USEMODULE += netstats_l2
USEMODULE += netstats_ipv6

include $(RIOTBASE)/Makefile.include
