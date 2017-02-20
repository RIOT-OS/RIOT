APPLICATION = lwip_sock_udp

include ../Makefile.tests_common

# lwIP's memory management doesn't seem to work on non 32-bit platforms at the
# moment.
BOARD_BLACKLIST := arduino-uno arduino-duemilanove arduino-mega2560 chronos \
                   msb-430 msb-430h telosb waspmote-pro wsn430-v1_3b \
                   wsn430-v1_4 z1
BOARD_INSUFFICIENT_MEMORY = nucleo32-f042 nucleo-f030 nucleo32-f031 nucleo-f042 \
                            nucleo-f334 nucleo-l053 stm32f0discovery weio

LWIP_IPV4 ?= 0

ifneq (0, $(LWIP_IPV4))
  USEMODULE += ipv4_addr
  USEMODULE += lwip_arp
  USEMODULE += lwip_ipv4
  CFLAGS += -DETHARP_SUPPORT_STATIC_ENTRIES=1
  LWIP_IPV6 ?= 0
else
  LWIP_IPV6 ?= 1
endif

ifneq (0, $(LWIP_IPV6))
  USEMODULE += ipv6_addr
  USEMODULE += lwip_ipv6_autoconfig
endif

USEMODULE += inet_csum
USEMODULE += lwip_ethernet lwip_netdev2
USEMODULE += lwip_sock_udp
USEMODULE += netdev2_eth
USEMODULE += netdev2_test
USEMODULE += ps

DISABLE_MODULE += auto_init

CFLAGS += -DDEVELHELP
CFLAGS += -DSO_REUSE
CFLAGS += -DLWIP_SO_RCVTIMEO

include $(RIOTBASE)/Makefile.include

test:
	./tests/01-run.py
