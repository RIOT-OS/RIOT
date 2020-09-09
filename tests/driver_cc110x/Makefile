BOARD ?= msba2
include ../Makefile.tests_common

DEVICE ?= cc1100 # The MSB-A2 uses the CC1100. New boards use CC1101

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
