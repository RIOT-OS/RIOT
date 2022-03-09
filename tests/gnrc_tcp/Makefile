include ../Makefile.tests_common

# Basic Configuration
BOARD ?= native
TAP ?= tap0

# Enable experimental feature "Dynamic MSL" to speedup testing
ENABLE_DYNAMIC_MSL ?= 1

# Shorten default connection TCP timeout to speedup testing
TIMEOUT_MS ?= 3000

# Set custom GNRC_TCP_NO_TIMEOUT constant for testing purposes
CUSTOM_GNRC_TCP_NO_TIMEOUT ?= 1

# This test depends on tap device setup (only allowed by root)
# Suppress test execution to avoid CI errors
TEST_ON_CI_BLACKLIST += all

ifeq (native,$(BOARD))
  TERMFLAGS ?= $(TAP)
else
  ETHOS_BAUDRATE ?= 115200
  CFLAGS += -DETHOS_BAUDRATE=$(ETHOS_BAUDRATE)
  TERMDEPS += ethos
  TERMPROG ?= sudo $(RIOTTOOLS)/ethos/ethos
  TERMFLAGS ?= $(TAP) $(PORT) $(ETHOS_BAUDRATE)
endif

USEMODULE += auto_init_gnrc_netif
USEMODULE += gnrc_ipv6_default
USEMODULE += gnrc_tcp
USEMODULE += gnrc_pktbuf_cmd
USEMODULE += gnrc_netif_single    # Only one interface used and it makes
                                  # shell commands easier
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += od

# Export used tap device to environment
export TAPDEV = $(TAP)

.PHONY: ethos

ethos:
	$(Q)env -u CC -u CFLAGS $(MAKE) -C $(RIOTTOOLS)/ethos

include $(RIOTBASE)/Makefile.include

# Set CONFIG_GNRC_TCP_CONNECTION_TIMEOUT_DURATION via CFLAGS if not being set
# via Kconfig
ifndef CONFIG_GNRC_TCP_CONNECTION_TIMEOUT_DURATION_MS
  CFLAGS += -DCONFIG_GNRC_TCP_CONNECTION_TIMEOUT_DURATION_MS=$(TIMEOUT_MS)
endif

# Set CONFIG_GNRC_TCP_EXPERIMENTAL_DYN_MSL_EN via CFLAGS if not being set
# via Kconfig
ifndef CONFIG_GNRC_TCP_EXPERIMENTAL_DYN_MSL_EN
  CFLAGS += -DCONFIG_GNRC_TCP_EXPERIMENTAL_DYN_MSL_EN=$(ENABLE_DYNAMIC_MSL)
endif

# Set the shell echo configuration via CFLAGS if not being controlled via Kconfig
ifndef CONFIG_KCONFIG_USEMODULE_SHELL
  CFLAGS += -DCONFIG_SHELL_NO_ECHO
endif

# Set GNRC_TCP_NO_TIMEOUT via CFLAGS
ifndef GNRC_TCP_NO_TIMEOUT
  CFLAGS += -DGNRC_TCP_NO_TIMEOUT=$(CUSTOM_GNRC_TCP_NO_TIMEOUT)
endif
