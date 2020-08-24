include ../Makefile.tests_common

BOARD ?= native

RIOTBASE ?= $(CURDIR)/../..

USEMODULE += shell
USEMODULE += shell_commands

USEMODULE += auto_init_can
USEMODULE += conn_can
USEMODULE += can_trx

FEATURES_REQUIRED += periph_can
FEATURES_REQUIRED += periph_gpio_irq

USEMODULE += schedstatistics

TRX_TO_BUILD ?= ncv7356

ifneq (,$(filter ncv7356,$(TRX_TO_BUILD)))
  USEMODULE += ncv7356
	NCV7356_MODE0_PIN ?= GPIO_PIN\(PORT_D,14\)
	NCV7356_MODE1_PIN ?= GPIO_PIN\(PORT_D,15\)

	CFLAGS += -DNCV7356_MODE0_PIN=$(NCV7356_MODE0_PIN)
	CFLAGS += -DNCV7356_MODE1_PIN=$(NCV7356_MODE1_PIN)
endif

GATEWAY_OPERATING_MODE ?= 0

ifneq (0, $(GATEWAY_OPERATING_MODE))
  USEMODULE += ipv4_addr
  USEMODULE += lwip_arp
  USEMODULE += lwip_ipv4
  USEMODULE += lwip_dhcp_auto
  USEMODULE += lwip lwip_sock_ip lwip_netdev
  USEMODULE += lwip_udp lwip_sock_udp
  USEMODULE += netdev_default
  CFLAGS += -DETHARP_SUPPORT_STATIC_ENTRIES=1
  CFLAGS += -DGATEWAY_OPERATING_MODE=$(GATEWAY_OPERATING_MODE)
  LWIP_IPV6 ?= 0
else
  USEMODULE += gnrc
  USEMODULE += gnrc_netdev_default
  USEMODULE += auto_init_gnrc_netif
  USEMODULE += gnrc_ipv6
  USEMODULE += gnrc_sock_udp
  USEMODULE += gnrc_nettype_custom
  CFLAGS += -DGNRC_NETIF_SINGLE=1
endif

SERVER_PORT ?= 42810
CAN_BITRATE ?= 83333
CAN_SAMPLE_POINT ?= 875

CFLAGS += -DSERVER_PORT=$(SERVER_PORT)
CFLAGS += -DCAN_BITRATE=$(CAN_BITRATE)
CFLAGS += -DCAN_SAMPLE_POINT=$(CAN_SAMPLE_POINT)

DEVELHELP ?= 1

QUIET ?= 1

include $(RIOTBASE)/Makefile.include
