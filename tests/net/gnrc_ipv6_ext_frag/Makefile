DEVELHELP := 1
# name of your application
include ../Makefile.net_common

export TAP ?= tap0

CFLAGS += -DOUTPUT=TEXT
CFLAGS += -DTEST_SUITES="gnrc_ipv6_ext_frag"

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
# add dummy interface to test forwarding to smaller MTU
USEMODULE += netdev_test
# Specify the mandatory networking modules for IPv6
USEMODULE += gnrc_ipv6_router_default
USEMODULE += gnrc_icmpv6_error
USEMODULE += gnrc_pktdump
USEMODULE += shell_cmd_gnrc_pktbuf
# IPv6 extension headers
USEMODULE += gnrc_ipv6_ext_frag
# UDP support for payload
USEMODULE += gnrc_udp
USEMODULE += od
# Add unittest framework
USEMODULE += embunit
# Add also the shell, some shell commands
USEMODULE += shell
USEMODULE += shell_cmds_default
USEMODULE += ps

# The test requires some setup and to be run as root
# So it cannot currently be run
TEST_ON_CI_BLACKLIST += all

# As there is an 'app.config' we want to explicitly disable Kconfig by setting
# the variable to empty
SHOULD_RUN_KCONFIG ?=

.PHONY: ethos

ethos:
	$(Q)env -u CC -u CFLAGS $(MAKE) -C $(RIOTTOOLS)/ethos

include $(RIOTBASE)/Makefile.include

# Set the pool size for limit objects if not being set by Kconfig
ifndef CONFIG_GNRC_IPV6_EXT_FRAG_LIMITS_POOL_SIZE
  CFLAGS += -DCONFIG_GNRC_IPV6_EXT_FRAG_LIMITS_POOL_SIZE=3
endif
