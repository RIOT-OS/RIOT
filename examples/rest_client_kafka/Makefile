# name of your application
APPLICATION = rest-client-kafka

# include config files for tinydtls and wolfMQTT
INCLUDES += -I$(CURDIR)/include

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

WIFI_SSID ?= "WIFI_SSID"
WIFI_PASS ?= "WIFI_PASS"

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

# Change this to 0 to show compiler invocation lines by default
QUIET ?= 1

# Print for helpful information when hitting stack overflow
# USEMODULE += ps

USEMODULE += rest_client

USEPKG    += tinycbor
USEMODULE += ztimer_msec

ENABLE_DEBUG ?= 1
CFLAGS += -DREST_CLIENT_ENABLE_DEBUG=$(ENABLE_DEBUG)
ifneq (0,$(ENABLE_DEBUG))
  USEMODULE += rest_client_debug
endif

# for now only one should be possible at the same time
TRANSPORT_COAP ?= 0
TRANSPORT_COAP_SECURE ?= 0
TRANSPORT_MQTT ?= 0
TRANSPORT_MQTT_SN ?= 0

ifneq (0,$(TRANSPORT_COAP))
  USEMODULE += rest_client_transport_coap
endif

ifneq (0,$(TRANSPORT_COAP_SECURE))
  USEMODULE += rest_client_transport_coaps
endif

ifneq (0,$(TRANSPORT_MQTT))
  USEMODULE += rest_client_transport_mqtt
endif

ifneq (0,$(TRANSPORT_MQTT_SN))
  USEMODULE += rest_client_transport_mqttsn
endif

# general coap settings
ifneq (,$(filter 1, $(TRANSPORT_COAP) $(TRANSPORT_COAP_SECURE)))
  CFLAGS += -DCONFIG_GCOAP_PDU_BUF_SIZE=1152
endif

QOS ?= 2
CFLAGS += -DCONFIG_QOS=$(QOS)

# we need to use user_settings.h to control the wolfMQTT package
CFLAGS += -DWOLFMQTT_USER_SETTINGS=1

IPV4 ?= 0
IPV6 ?= 1

USEMODULE += lwip_netdev

ifneq (0,$(IPV4))
  USEMODULE += ipv4_addr
  USEMODULE += lwip_arp
  USEMODULE += lwip_ipv4
  USEMODULE += lwip_dhcp_auto
  CFLAGS += -DETHARP_SUPPORT_STATIC_ENTRIES=1

  # not included when using IPv4-only, but required by CoAP
  ifneq (, $(filter 1, $(TRANSPORT_COAP) $(TRANSPORT_COAP_SECURE)))
    USEMODULE += random
  endif

  # Todo: only temporary until rebase
  CFLAGS += -DSOCK_HAS_IPV4
endif

ifneq (0,$(IPV6))
  USEMODULE += ipv6_addr
  USEMODULE += lwip_ipv6
  USEMODULE += lwip_ipv6_autoconfig
endif

# Instead of simulating an Ethernet connection, we can also simulate
# an IEEE 802.15.4 radio using ZEP
USE_ZEP ?= 0

# set the ZEP port for native
ZEP_PORT_BASE ?= 17754
ifeq (1,$(USE_ZEP))
  TERMFLAGS += -z [::1]:$(ZEP_PORT_BASE)
  USEMODULE += socket_zep

  ifneq (,$(ZEP_MAC))
    TERMFLAGS += --eui64=$(ZEP_MAC)
  endif
endif

include $(RIOTBASE)/Makefile.include

# needs to be put after "include $(RIOTBASE)/Makefile.include"
ifneq (,$(filter arch_esp,$(FEATURES_USED)))
  CFLAGS += -DESP_WIFI_SSID=\"$(WIFI_SSID)\"
  CFLAGS += -DESP_WIFI_PASS=\"$(WIFI_PASS)\"
endif
