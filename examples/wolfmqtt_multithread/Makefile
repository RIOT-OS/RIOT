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

# This firmware is implemented inside the package examples
USEMODULE += wolfmqtt_examples_multithread

LWIP_IPV4 ?= 1
LWIP_IPV6 ?= 1

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

USEMODULE += auto_init_sock_dns

include $(RIOTBASE)/Makefile.include

# needs to be put after "include $(RIOTBASE)/Makefile.include"
ifneq (,$(filter arch_esp,$(FEATURES_USED)))
  CFLAGS += -DESP_WIFI_SSID=\"$(WIFI_SSID)\"
  CFLAGS += -DESP_WIFI_PASS=\"$(WIFI_PASS)\"
endif
