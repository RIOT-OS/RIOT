include ../Makefile.net_common

# Basic Configuration
BOARD ?= native
TAP ?= tap0

# Shorten default TCP timeouts to speedup testing
MSL_MS ?= 1000
TIMEOUT_MS ?= 3000

# This test depends on tap device setup (only allowed by root)
# Suppress test execution to avoid CI errors
TEST_ON_CI_BLACKLIST += all

ifneq (,$(filter native native32 native64,$(BOARD)))
  PORT ?= $(TAP)
else
  ETHOS_BAUDRATE ?= 115200
  CFLAGS += -DETHOS_BAUDRATE=$(ETHOS_BAUDRATE)
  TERMDEPS += ethos
  TERMPROG ?= sudo $(RIOTTOOLS)/ethos/ethos
  TERMFLAGS ?= $(TAP) $(PORT) $(ETHOS_BAUDRATE)
endif

USEMODULE += auto_init_gnrc_netif
USEMODULE += gnrc_ipv6_default
USEMODULE += gnrc_sock_tcp
USEMODULE += shell_cmd_gnrc_pktbuf
USEMODULE += gnrc_netif_single          # Only one interface used and it makes
                                        # shell commands easier
USEMODULE += shell
USEMODULE += shell_cmds_default
USEMODULE += od

# Export used tap device to environment
export TAPDEV = $(TAP)

.PHONY: ethos

ethos:
	$(Q)env -u CC -u CFLAGS $(MAKE) -C $(RIOTTOOLS)/ethos

include $(RIOTBASE)/Makefile.include

# Set CONFIG_GNRC_TCP_MSL via CFLAGS if not being set via Kconfig
ifndef CONFIG_GNRC_TCP_MSL_MS
  CFLAGS += -DCONFIG_GNRC_TCP_MSL_MS=$(MSL_MS)
endif

# Set CONFIG_GNRC_TCP_CONNECTION_TIMEOUT_DURATION via CFLAGS if not being set
# via Kconfig
ifndef CONFIG_GNRC_TCP_CONNECTION_TIMEOUT_DURATION_MS
  CFLAGS += -DCONFIG_GNRC_TCP_CONNECTION_TIMEOUT_DURATION_MS=$(TIMEOUT_MS)
endif

# Set the shell echo configuration via CFLAGS if not being controlled via Kconfig
ifndef CONFIG_KCONFIG_USEMODULE_SHELL
  CFLAGS += -DCONFIG_SHELL_NO_ECHO
endif
