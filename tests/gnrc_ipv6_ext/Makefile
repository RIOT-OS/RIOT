# name of your application
APPLICATION = gnrc_ipv6_ext
include ../Makefile.tests_common

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

BOARD_INSUFFICIENT_MEMORY := airfy-beacon chronos maple-mini msb-430 maple-mini msb-430h \
                          nrf51dongle nrf6310 nucleo-f103 nucleo-f334 pca10000 pca10005 \
                          spark-core stm32f0discovery telosb weio wsn430-v1_3b \
                          wsn430-v1_4 yunjia-nrf51822 z1 nucleo-f030 nucleo32-f042 \
                          nucleo32-f031 nucleo-l053

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

# This requires ENABLE_DEBUG in gnrc_ipv6.c to be 1
test:
# `testrunner` calls `make term` recursively, results in duplicated `TERMFLAGS`.
# So clears `TERMFLAGS` before run.
	TERMFLAGS= tests/01-run.py
