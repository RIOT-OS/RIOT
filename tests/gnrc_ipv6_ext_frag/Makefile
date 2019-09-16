DEVELHELP := 1
# name of your application
include ../Makefile.tests_common

BOARD_INSUFFICIENT_MEMORY := arduino-duemilanove arduino-leonardo \
                             arduino-mega2560 arduino-nano arduino-uno chronos \
                             i-nucleo-lrwan1 mega-xplained msb-430 msb-430h \
                             nucleo-f030r8 nucleo-f031k6 nucleo-f042k6 \
                             nucleo-f303k8 nucleo-f334r8 nucleo-l031k6 \
                             nucleo-l053r8 stm32f0discovery stm32l0538-disco \
                             telosb waspmote-pro wsn430-v1_3b wsn430-v1_4 z1

export TAP ?= tap0

CFLAGS += -DOUTPUT=TEXT
CFLAGS += -DTEST_SUITES="gnrc_ipv6_ext_frag"
CFLAGS += -DGNRC_IPV6_EXT_FRAG_LIMITS_POOL_SIZE=3

# use Ethernet as link-layer protocol for native
# The only current general option for non-native boards, ethos, performs poorly
# with the rapidly sent, large packets sent by the Linux kernel.
ifeq (native,$(BOARD))
  USEMODULE += netdev_tap
  TERMFLAGS ?= $(TAP)

  USEMODULE += auto_init_gnrc_netif
endif
# Specify the mandatory networking modules for IPv6
USEMODULE += gnrc_ipv6_router_default
USEMODULE += gnrc_icmpv6_error
USEMODULE += gnrc_pktdump
USEMODULE += gnrc_pktbuf_cmd
# IPv6 extension headers
USEMODULE += gnrc_ipv6_ext_frag
# UDP support for payload
USEMODULE += gnrc_udp
USEMODULE += od
# Add unittest framework
USEMODULE += embunit
# Add also the shell, some shell commands
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps

# native requires sudo for the `scapy` tests, but those are not executed for
# non-native boards
TEST_ON_CI_BLACKLIST += native

include $(RIOTBASE)/Makefile.include
