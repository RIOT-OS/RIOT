# name of your application
include ../Makefile.tests_common

BOARD_INSUFFICIENT_MEMORY := chronos msb-430 msb-430h nucleo-f030r8 \
                             nucleo-f031k6 nucleo-f042k6 nucleo-f303k8 \
                             nucleo-f334r8 nucleo-l031k6 nucleo-l053r8 \
                             stm32f0discovery

# use Ethernet as link-layer protocol
USEMODULE += netdev_eth
USEMODULE += netdev_test
# Specify the mandatory networking modules for IPv6
USEMODULE += gnrc_ipv6_router_default
# IPv6 extension headers
USEMODULE += gnrc_ipv6_ext
USEMODULE += gnrc_rpl_srh
USEMODULE += gnrc_sixlowpan_iphc_nhc
# UDP
USEMODULE += gnrc_udp
# Add also the shell, some shell commands
USEMODULE += ps

CFLAGS += -DGNRC_NETIF_IPV6_ADDRS_NUMOF=3

TEST_ON_CI_WHITELIST += all

include $(RIOTBASE)/Makefile.include

# The test can check more things with ENABLE_DEBUG set to 1 in gnrc_ipv6.c
test:
	tests/01-run.py
