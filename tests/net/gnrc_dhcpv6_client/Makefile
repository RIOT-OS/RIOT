include ../Makefile.net_common

# generate random free port
DHCPV6_SERVER_PORT := 61342

RIOTBASE ?= $(CURDIR)/../..

# boards don't support ethos
BOARD_BLACKLIST += ruuvitag thingy52

USEMODULE += dhcpv6_client_ia_pd
USEMODULE += dhcpv6_client_ia_na
USEMODULE += gnrc_dhcpv6_client
USEMODULE += gnrc_ipv6_default
USEMODULE += xtimer
ifneq (,$(filter native native32,$(BOARD)))
  # Has to be provided here and not in Makefile.dep, so TERMFLAGS are properly
  # configured
  USEMODULE += netdev_default
  IFACE ?= tapbr0
else
  IFACE ?= tap0
  ETHOS_BAUDRATE ?= 115200
  CFLAGS += -DETHOS_BAUDRATE=$(ETHOS_BAUDRATE)
  TERMPROG ?= sudo $(RIOTBASE)/dist/tools/ethos/ethos
  TERMFLAGS ?= $(IFACE) $(PORT) $(ETHOS_BAUDRATE)
  TERMDEPS += ethos
endif
USEMODULE += auto_init_gnrc_netif

USEMODULE += shell_cmds_default

CFLAGS += -DDHCPV6_SERVER_PORT=$(DHCPV6_SERVER_PORT)

TEST_DEPS += dhcpv6_server

# The test requires to be run as root
TEST_ON_CI_BLACKLIST += all

include $(RIOTBASE)/Makefile.include

.PHONY: dhcpv6_server

# export IFACE variable to environment of dhcpv6_server and
# test-with-config/check-config rules
# see: https://doc.riot-os.org/build-system-basics.html#variable-declaration-guidelines
$(call target-export-variables,dhcpv6_server test-with-config/check-config,IFACE)

dhcpv6_server:
dhcpv6_server:
	$(CURDIR)/dhcpv6_server.sh $(DHCPV6_SERVER_PORT) $(CURDIR)/kea-dhcp6.conf

ifeq (,$(filter native native32,$(BOARD)))
.PHONY: ethos

ethos:
	$(Q)env -u CC -u CFLAGS $(MAKE) -C $(RIOTBASE)/dist/tools/ethos
endif
