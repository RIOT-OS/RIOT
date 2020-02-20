include ../Makefile.tests_common

LWIP_IPV4 ?= 0

ifneq (0, $(LWIP_IPV4))
  USEMODULE += ipv4_addr
  USEMODULE += lwip_arp
  USEMODULE += lwip_ipv4
  USEMODULE += lwip_dhcp_auto
  CFLAGS += -DETHARP_SUPPORT_STATIC_ENTRIES=1
  LWIP_IPV6 ?= 0
else
  USEMODULE += ipv6_addr
  USEMODULE += lwip_ipv6
  USEMODULE += lwip_ipv6_autoconfig
  LWIP_IPV6 ?= 1
endif

# including lwip_ipv6_mld would currently break this test on at86rf2xx radios
USEMODULE += lwip lwip_sock_ip lwip_netdev
USEMODULE += lwip_udp lwip_sock_udp
USEMODULE += lwip_tcp lwip_sock_tcp
USEMODULE += lwip_sock_async
USEMODULE += sock_async_event
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps
USEMODULE += od
USEMODULE += netdev_default

ifeq ($(BOARD),native)
  USEMODULE += lwip_ethernet
endif

# Test only implemented for native
ifneq ($(BOARD),native)
  TESTS=
endif

DISABLE_MODULE += test_utils_interactive_sync

# test fails on murdock and on my machine due to the process exiting directly
TEST_ON_CI_BLACKLIST += all

include $(RIOTBASE)/Makefile.include
