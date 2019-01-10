include ../Makefile.tests_common

BOARD_INSUFFICIENT_MEMORY := arduino-duemilanove arduino-mega2560 arduino-uno \
                             chronos mega-xplained msb-430 msb-430h nucleo-f030r8 \
                             nucleo-f031k6 nucleo-f042k6 nucleo-f303k8 \
                             nucleo-f334r8 nucleo-l031k6 nucleo-l053r8 \
                             stm32f0discovery telosb waspmote-pro z1

# Include packages that pull up and auto-init the link layer.
# NOTE: 6LoWPAN will be included if IEEE802.15.4 devices are present
USEMODULE += gnrc_netdev_default
USEMODULE += auto_init_gnrc_netif
# Specify the mandatory networking modules
USEMODULE += gnrc_ipv6_default
USEMODULE += gnrc_udp
USEMODULE += gnrc_sock_udp

USEMODULE += nanocoap_sock

# Required by test
USEMODULE += od
USEMODULE += shell

include $(RIOTBASE)/Makefile.include
