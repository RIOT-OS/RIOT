include ../Makefile.net_common

RIOTBASE ?= $(CURDIR)/../..

export TAP ?= tap0

USEMODULE += auto_init_gnrc_netif
USEMODULE += ipv4_addr
USEMODULE += ipv6_addr
USEMODULE += sock_dodtls
USEMODULE += gnrc_ipv6_default
USEMODULE += gnrc_netif_single          # Only one interface used and it makes
                                        # shell commands easier
USEMODULE += posix_inet
# tinydtls needs crypto secure PRNG
USEMODULE += prng_sha1prng
USEMODULE += shell
USEMODULE += shell_cmds_default

USEPKG += tinydtls

# use Ethernet as link-layer protocol
ifneq (,$(filter native32 native64,$(BOARD)))
  PORT ?= $(TAP)
else
  ETHOS_BAUDRATE ?= 115200
  CFLAGS += -DETHOS_BAUDRATE=$(ETHOS_BAUDRATE)
  TERMDEPS += ethos
  TERMPROG ?= sudo $(RIOTTOOLS)/ethos/ethos
  TERMFLAGS ?= $(TAP) $(PORT) $(ETHOS_BAUDRATE)
endif

CFLAGS += -DTHREAD_STACKSIZE_MAIN=\(3*THREAD_STACKSIZE_DEFAULT\)

# The test requires some setup and to be run as root
# So it cannot currently be run
TEST_ON_CI_BLACKLIST += all

.PHONY: ethos

ethos:
	$(Q)env -u CC -u CFLAGS $(MAKE) -C $(RIOTTOOLS)/ethos

include $(RIOTBASE)/Makefile.include
