BOARD ?= samr21-xpro

include ../Makefile.tests_common

# Basic Configuration
BOARD ?= native
TAP ?= tap0

# Shorten default TCP timeouts to speedup testing
MSL_US ?= 1000000
TIMEOUT_US ?= 3000000

# unsupported by ethos: chronos, hamilton, ruuvitag
BOARD_BLACKLIST := chronos hamilton ruuvitag thingy52

# This test depends on tap device setup (only allowed by root)
# Suppress test execution to avoid CI errors
TEST_ON_CI_BLACKLIST += all

CFLAGS += -DSHELL_NO_ECHO
CFLAGS += -DGNRC_TCP_MSL=$(MSL_US)
CFLAGS += -DGNRC_TCP_CONNECTION_TIMEOUT_DURATION=$(TIMEOUT_US)

ifeq (native,$(BOARD))
  USEMODULE += netdev_tap
  TERMFLAGS ?= $(TAP)
else
  USEMODULE += stdio_ethos
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
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += od

# Export used tap device to environment
export TAPDEV = $(TAP)

.PHONY: ethos

ethos:
	$(Q)env -u CC -u CFLAGS make -C $(RIOTTOOLS)/ethos

include $(RIOTBASE)/Makefile.include
