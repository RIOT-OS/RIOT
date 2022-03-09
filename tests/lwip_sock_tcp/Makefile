include ../Makefile.tests_common

LWIP_IPV4 ?= 0
LWIP_IPV6 ?= 1

ifneq (0, $(LWIP_IPV4))
  USEMODULE += ipv4_addr
  USEMODULE += lwip_arp
  USEMODULE += lwip_ipv4
  CFLAGS += -DETHARP_SUPPORT_STATIC_ENTRIES=1
endif

ifneq (0, $(LWIP_IPV6))
  USEMODULE += ipv6_addr
  USEMODULE += lwip_ipv6_autoconfig
endif

USEMODULE += inet_csum
USEMODULE += lwip_netdev
USEMODULE += netdev_eth
USEMODULE += netdev_test
USEMODULE += ps
USEMODULE += sock_tcp

DISABLE_MODULE += auto_init_lwip

CFLAGS += -DSO_REUSE
CFLAGS += -DLWIP_SO_RCVTIMEO
CFLAGS += -DLWIP_SOCK_TCP_ACCEPT_TIMEOUT=500
CFLAGS += -DLWIP_NETIF_LOOPBACK=1
CFLAGS += -DLWIP_HAVE_LOOPIF=1

include $(RIOTBASE)/Makefile.include
