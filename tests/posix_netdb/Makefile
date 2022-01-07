include ../Makefile.tests_common

WIFI_SSID ?= "WIFI_SSID"
WIFI_PASS ?= "WIFI_PASS"

USEMODULE += ztimer
USEMODULE += ztimer_sec

USEMODULE += posix_netdb

USEMODULE += auto_init_sock_dns

LWIP_IPV4 ?= 1
LWIP_IPV6 ?= 1

USEMODULE += lwip
USEMODULE += lwip_netdev

ifneq (0,$(LWIP_IPV4))
  USEMODULE += ipv4_addr
  USEMODULE += lwip_arp
  USEMODULE += lwip_ipv4
  USEMODULE += lwip_dhcp_auto
  CFLAGS += -DETHARP_SUPPORT_STATIC_ENTRIES=1
endif

ifneq (0,$(LWIP_IPV6))
  USEMODULE += ipv6_addr
  USEMODULE += lwip_ipv6
  USEMODULE += lwip_ipv6_autoconfig
endif

# The test requires some setup (active internet connection) so it cannot
# currently be run
TEST_ON_CI_BLACKLIST += all

include $(RIOTBASE)/Makefile.include

# needs to be put after "include $(RIOTBASE)/Makefile.include"
ifneq (,$(filter arch_esp,$(FEATURES_USED)))
  CFLAGS += -DESP_WIFI_SSID=\"$(WIFI_SSID)\"
  CFLAGS += -DESP_WIFI_PASS=\"$(WIFI_PASS)\"
endif
