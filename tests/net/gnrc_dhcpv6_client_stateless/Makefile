DEVELHELP := 1
include ../Makefile.net_common

export TAP ?= tap0

USEMODULE += auto_init_gnrc_netif
USEMODULE += dhcpv6_client_mud_url
USEMODULE += gnrc_dhcpv6_client
USEMODULE += gnrc_ipv6_default
USEMODULE += auto_init_dhcpv6_client
USEMODULE += netdev_default
USEMODULE += gnrc_pktdump
USEMODULE += ps
USEMODULE += shell
USEMODULE += shell_cmds_default

# use Ethernet as link-layer protocol
ifneq (,$(filter native32 native64,$(BOARD)))
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

# The test requires some setup and to be run as root
# So it cannot currently be run on CI
TEST_ON_CI_BLACKLIST += all

# As there is an 'app.config' we want to explicitly disable Kconfig by setting
# the variable to empty
SHOULD_RUN_KCONFIG ?=

include $(RIOTBASE)/Makefile.include


ifeq (,$(filter native32 native64,$(BOARD)))
.PHONY: ethos

ethos:
	$(Q)env -u CC -u CFLAGS $(MAKE) -C $(RIOTBASE)/dist/tools/ethos
endif
