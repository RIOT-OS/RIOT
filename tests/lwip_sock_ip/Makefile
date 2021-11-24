include ../Makefile.tests_common

LWIP_IPV4 ?= 0
LWIP_IPV6 ?= 1
AUX_LOCAL ?= 1

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

ifeq (1, $(AUX_LOCAL))
  USEMODULE += sock_aux_local
endif

USEMODULE += inet_csum
USEMODULE += l2util
USEMODULE += lwip_netdev
USEMODULE += netdev_eth
USEMODULE += netdev_test
USEMODULE += ps
USEMODULE += sock_ip

DISABLE_MODULE += auto_init_lwip

CFLAGS += -DSO_REUSE
CFLAGS += -DLWIP_SO_RCVTIMEO

include $(RIOTBASE)/Makefile.include
