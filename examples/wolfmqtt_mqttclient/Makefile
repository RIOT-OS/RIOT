# name of your application
APPLICATION = wolfmqtt_mqttclient

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

WIFI_SSID ?= "WIFI_SSID"
WIFI_PASS ?= "WIFI_PASS"

USEPKG += wolfmqtt

# This firmware is implemented inside the package examples
USEMODULE += wolfmqtt_examples_mqttclient

LWIP_IPV4 ?= 0

USEMODULE += lwip
USEMODULE += lwip_netdev
USEMODULE += lwip_netdb

ifneq (0,$(LWIP_IPV4))
  USEMODULE += ipv4_addr
  USEMODULE += lwip_arp
  USEMODULE += lwip_ipv4
  USEMODULE += lwip_dhcp_auto
  CFLAGS += -DETHARP_SUPPORT_STATIC_ENTRIES=1
  LWIP_IPV6 ?= 0
else
  LWIP_IPV6 ?= 1
endif

ifneq (0,$(LWIP_IPV6))
  USEMODULE += ipv6_addr
  USEMODULE += lwip_ipv6
  USEMODULE += lwip_ipv6_autoconfig
endif

USEMODULE += sock_udp
USEMODULE += sock_tcp

include $(RIOTBASE)/Makefile.include

# needs to be put after "include $(RIOTBASE)/Makefile.include"
ifneq (,$(filter arch_esp,$(FEATURES_USED)))
  CFLAGS += -DESP_WIFI_SSID=\"$(WIFI_SSID)\"
  CFLAGS += -DESP_WIFI_PASS=\"$(WIFI_PASS)\"
endif
