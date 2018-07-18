include ../Makefile.tests_common

RIOTBASE ?= $(CURDIR)/../..

BOARD_INSUFFICIENT_MEMORY := chronos telosb nucleo-f042k6 nucleo-f031k6 \
                             nucleo-f030r8 nucleo-f303k8 nucleo-l053r8 \
                             nucleo-l031k6 stm32f0discovery z1

USEMODULE += sock_dns
USEMODULE += gnrc_sock_udp
USEMODULE += gnrc_ipv6_default
USEMODULE += gnrc_ipv6_nib_dns
USEMODULE += gnrc_netdev_default
USEMODULE += auto_init_gnrc_netif

USEMODULE += shell_commands

USEMODULE += posix

LOW_MEMORY_BOARDS := nucleo-f334r8 msb-430 msb-430h

ifeq ($(BOARD),$(filter $(BOARD),$(LOW_MEMORY_BOARDS)))
    CFLAGS += -DGNRC_PKTBUF_SIZE=512 -DGNRC_NETIF_IPV6_ADDRS_NUMOF=2 \
              -DGNRC_NETIF_IPV6_GROUPS_NUMOF=2 -DGNRC_IPV6_NIB_NUMOF=1 \
              -DNRC_IPV6_NIB_OFFL_NUMOF=1
endif

include $(RIOTBASE)/Makefile.include
