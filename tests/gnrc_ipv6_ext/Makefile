# name of your application
include ../Makefile.tests_common

BOARD_INSUFFICIENT_MEMORY := airfy-beacon chronos maple-mini msb-430 msb-430h \
                             nrf51dongle nrf6310 nucleo-f031k6 nucleo-f042k6 \
                             nucleo-l031k6 nucleo-f030r8 nucleo-f103rb nucleo-f334r8 nucleo-l053r8 \
                             spark-core stm32f0discovery telosb \
                             wsn430-v1_3b wsn430-v1_4 yunjia-nrf51822 z1

# Include packages that pull up and auto-init the link layer.
# NOTE: 6LoWPAN will be included if IEEE802.15.4 devices are present
USEMODULE += gnrc_netdev_default
USEMODULE += auto_init_gnrc_netif
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

include $(RIOTBASE)/Makefile.include

# The test can check more things with ENABLE_DEBUG set to 1 in gnrc_ipv6.c
test:
	tests/01-run.py
