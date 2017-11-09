# name of your application
APPLICATION = gnrc_ipv6_ext
include ../Makefile.tests_common

BOARD_INSUFFICIENT_MEMORY := airfy-beacon chronos maple-mini msb-430 msb-430h \
                             nrf51dongle nrf6310 nucleo32-f031 nucleo32-f042 \
                             nucleo32-l031 nucleo-f030 nucleo-f103 nucleo-f334 nucleo-l053 \
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

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
CFLAGS += -DDEVELHELP

include $(RIOTBASE)/Makefile.include

# The test can check more things with ENABLE_DEBUG set to 1 in gnrc_ipv6.c
test:
	tests/01-run.py
