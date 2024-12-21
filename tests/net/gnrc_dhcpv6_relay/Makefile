include ../Makefile.net_common

RIOTBASE ?= $(CURDIR)/../..

export TAP ?= tap0

USEMODULE += dhcpv6_relay
USEMODULE += event_thread
USEMODULE += gnrc_ipv6_default
USEMODULE += gnrc_netif_single          # Only one interface used and it makes
                                        # shell commands easier

# use Ethernet as link-layer protocol
ifneq (,$(filter native native32,$(BOARD)))
  PORT ?= $(TAP)
else
  ETHOS_BAUDRATE ?= 115200
  CFLAGS += -DETHOS_BAUDRATE=$(ETHOS_BAUDRATE)
  TERMDEPS += ethos
  TERMPROG ?= sudo $(RIOTTOOLS)/ethos/ethos
  TERMFLAGS ?= $(TAP) $(PORT) $(ETHOS_BAUDRATE)
endif
USEMODULE += auto_init_gnrc_netif

USEMODULE += ps
USEMODULE += shell
USEMODULE += shell_cmds_default

# The test requires some setup and to be run as root
# So it cannot currently be run
TEST_ON_CI_BLACKLIST += all

.PHONY: ethos

ethos:
	$(Q)env -u CC -u CFLAGS $(MAKE) -C $(RIOTTOOLS)/ethos

include $(RIOTBASE)/Makefile.include

ifndef CONFIG_DHCPV6_RELAY_HOP_LIMIT
  CONFIG_DHCPV6_RELAY_HOP_LIMIT=8
  CFLAGS += -DCONFIG_DHCPV6_RELAY_HOP_LIMIT=$(CONFIG_DHCPV6_RELAY_HOP_LIMIT)
endif
export CONFIG_DHCPV6_RELAY_HOP_LIMIT
ifndef CONFIG_DHCPV6_RELAY_BUFLEN
  CONFIG_DHCPV6_RELAY_BUFLEN=256
  CFLAGS += -DCONFIG_DHCPV6_RELAY_BUFLEN=$(CONFIG_DHCPV6_RELAY_BUFLEN)
endif
export CONFIG_DHCPV6_RELAY_BUFLEN
