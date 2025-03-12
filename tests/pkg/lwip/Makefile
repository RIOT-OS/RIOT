include ../Makefile.pkg_common

LWIP_IPV4 ?= 0
LWIP_IPV6 ?= 1

ifneq (0, $(LWIP_IPV4))
  USEMODULE += ipv4_addr
  USEMODULE += lwip_arp
  USEMODULE += lwip_ipv4
  USEMODULE += lwip_dhcp_auto
  CFLAGS += -DETHARP_SUPPORT_STATIC_ENTRIES=1
endif

ifneq (0, $(LWIP_IPV6))
  USEMODULE += ipv6_addr
  USEMODULE += lwip_ipv6
  USEMODULE += lwip_ipv6_autoconfig
endif

# including lwip_ipv6_mld would currently break this test on at86rf2xx radios
USEMODULE += lwip
USEMODULE += lwip_udp
USEMODULE += lwip_tcp
USEMODULE += sock_async_event
USEMODULE += sock_ip
USEMODULE += sock_tcp
USEMODULE += sock_udp
USEMODULE += sock_util
USEMODULE += shell
USEMODULE += shell_cmds_default
USEMODULE += ps
USEMODULE += od
USEMODULE += netdev_default
USEMODULE += ztimer_usec

ifneq (,$(filter native32 native64,$(BOARD)))
  USEMODULE += lwip_ethernet
endif

# Test only implemented for native
ifeq (,$(filter native32 native64,$(BOARD)))
  TESTS=
endif

DISABLE_MODULE += test_utils_interactive_sync

# test fails on murdock and on my machine due to the process exiting directly
TEST_ON_CI_BLACKLIST += all

include $(RIOTBASE)/Makefile.include

# Set a custom channel if needed
include $(RIOTMAKE)/default-radio-settings.inc.mk
