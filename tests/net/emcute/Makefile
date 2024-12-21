include ../Makefile.net_common

RIOTBASE ?= $(CURDIR)/../..

export TAP ?= tap0

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
USEMODULE += gnrc_ipv6_default
USEMODULE += sock_udp
USEMODULE += gnrc_netif_single          # Only one interface used and it makes
                                        # shell commands easier
USEMODULE += emcute
USEMODULE += od
USEMODULE += shell
USEMODULE += shell_cmds_default
USEMODULE += sock_util

CFLAGS += -DSTDIO_UART_RX_BUFSIZE="512" # Adapt to SHELL_BUFSIZE in app

# The test requires some setup and to be run as root
# So it cannot currently be run
TEST_ON_CI_BLACKLIST += all

.PHONY: ethos

ethos:
	$(Q)env -u CC -u CFLAGS $(MAKE) -C $(RIOTTOOLS)/ethos

include $(RIOTBASE)/Makefile.include

# Set CONFIG_EMCUTE_TOPIC_MAXLEN via CFLAGS if not being set via Kconfig.
ifndef CONFIG_EMCUTE_TOPIC_MAXLEN
  CFLAGS += -DCONFIG_EMCUTE_TOPIC_MAXLEN=249   # 256 - 7
endif
