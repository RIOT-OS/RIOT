include ../Makefile.tests_common

BOARD_INSUFFICIENT_MEMORY = i-nucleo-lrwan1 nucleo-f031k6 nucleo-f042k6 \
                            nucleo-l031k6 nucleo-f030r8 nucleo-f303k8 \
                            nucleo-f334r8 nucleo-l053r8 stm32f0discovery \
                            stm32l0538-disco

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
USEMODULE += l2util
USEMODULE += lwip_netdev
USEMODULE += lwip_sock_ip
USEMODULE += netdev_eth
USEMODULE += netdev_test
USEMODULE += ps

DISABLE_MODULE += auto_init

CFLAGS += -DSO_REUSE
CFLAGS += -DLWIP_SO_RCVTIMEO

include $(RIOTBASE)/Makefile.include
