DEVELHELP := 1
include $(CURDIR)/../Makefile.tests_common

export TAP ?= tap0

USEMODULE += auto_init_gnrc_netif
USEMODULE += dhcpv6_client_mud_url
USEMODULE += gnrc_dhcpv6_client_6lbr
USEMODULE += gnrc_netdev_default
USEMODULE += gnrc_pktdump
USEMODULE += gnrc_sixlowpan_border_router_default
USEMODULE += ps
USEMODULE += shell
USEMODULE += shell_commands

# use Ethernet as link-layer protocol
ifeq (native,$(BOARD))
  TERMFLAGS += -z [::1]:17754
else
  ETHOS_BAUDRATE ?= 115200
  CFLAGS += -DETHOS_BAUDRATE=$(ETHOS_BAUDRATE)
  TERMDEPS += ethos
  TERMPROG ?= sudo $(RIOTTOOLS)/ethos/ethos
  TERMFLAGS ?= $(TAP) $(PORT) $(ETHOS_BAUDRATE)
  STATIC_ROUTES ?= 1
endif

# The test requires some setup and to be run as root
# So it cannot currently be run on CI
TEST_ON_CI_BLACKLIST += all

# As there is an 'app.config' we want to explicitly disable Kconfig by setting
# the variable to empty
SHOULD_RUN_KCONFIG ?=

include $(RIOTBASE)/Makefile.include

ifndef CONFIG_GNRC_DHCPV6_CLIENT_6LBR_STATIC_ROUTE
ifeq (1,$(STATIC_ROUTES))
  CFLAGS += -DCONFIG_GNRC_DHCPV6_CLIENT_6LBR_STATIC_ROUTE=1
  # CONFIG_GNRC_DHCPV6_CLIENT_6LBR_STATIC_ROUTE=1 requires one more address for
  # `fe80::2`.
  CFLAGS += -DCONFIG_GNRC_NETIF_IPV6_ADDRS_NUMOF=3
endif
endif

ifeq (,$(filter native,$(BOARD)))
.PHONY: ethos

ethos:
	$(Q)env -u CC -u CFLAGS $(MAKE) -C $(RIOTBASE)/dist/tools/ethos
endif
