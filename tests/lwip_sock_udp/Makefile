APPLICATION = lwip_sock_udp

BOARD ?= native

RIOTBASE ?= $(CURDIR)/../..

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

QUIET ?= 1

include $(RIOTBASE)/Makefile.include

test:
	./tests/01-run.py
