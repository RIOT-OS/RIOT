include ../Makefile.tests_common

RIOTBASE ?= $(CURDIR)/../..

export TAP ?= tap0

USEPKG += paho-mqtt

NETWORK_STACK ?= lwip

# paho-mqtt depends on TCP support, choose the stack you want
ifeq (,$(filter lwip gnrc,$(NETWORK_STACK)))
  $(error Supported network stacks are `lwip` and `gnrc`)
endif

USEMODULE += sock_tcp

ifeq (lwip,$(NETWORK_STACK))
  USEMODULE += lwip_netdev
  USEMODULE += lwip

  LWIP_IPV4 ?= 0

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
    USEMODULE += lwip_ipv6_autoconfig
  endif
else ifeq (gnrc,$(NETWORK_STACK))
  USEMODULE += auto_init_gnrc_netif
  USEMODULE += gnrc_netif_single
  USEMODULE += gnrc_ipv6
endif

ifeq (native,$(BOARD))
  TERMFLAGS ?= $(TAP)
else
  ETHOS_BAUDRATE ?= 115200
  CFLAGS += -DETHOS_BAUDRATE=$(ETHOS_BAUDRATE)
  TERMDEPS += ethos
  TERMPROG ?= sudo $(RIOTTOOLS)/ethos/ethos
  TERMFLAGS ?= $(TAP) $(PORT) $(ETHOS_BAUDRATE)
endif

USEMODULE += shell
USEMODULE += shell_commands

# The test requires some setup and to be run as root
# So it cannot currently be run
TEST_ON_CI_BLACKLIST += all

.PHONY: ethos

ethos:
	$(Q)env -u CC -u CFLAGS $(MAKE) -C $(RIOTTOOLS)/ethos

include $(RIOTBASE)/Makefile.include
