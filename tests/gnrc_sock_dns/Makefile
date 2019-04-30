include ../Makefile.tests_common

RIOTBASE ?= $(CURDIR)/../..

BOARD_INSUFFICIENT_MEMORY := arduino-duemilanove arduino-mega2560 arduino-nano \
                             arduino-uno chronos hifive1 mega-xplained \
                             msb-430 msb-430h nucleo-f042k6 nucleo-f031k6 \
                             nucleo-f030r8 nucleo-f303k8 nucleo-l053r8 \
                             nucleo-l031k6 stm32f0discovery thingy52 telosb \
                             waspmote-pro wsn430-v1_3b wsn430-v1_4 z1
# chronos, hamilton mips-malta, and ruuvitag boards don't support ethos
BOARD_BLACKLIST := chronos hamilton mips-malta ruuvitag

export TAP ?= tap0

USEMODULE += sock_dns
USEMODULE += gnrc_sock_udp
USEMODULE += gnrc_ipv6_default
USEMODULE += gnrc_ipv6_nib_dns
# use Ethernet as link-layer protocol
ifeq (native,$(BOARD))
  USEMODULE += netdev_tap

  TERMFLAGS ?= $(TAP)
else
  USEMODULE += ethos

  ETHOS_BAUDRATE ?= 115200
  CFLAGS += -DETHOS_BAUDRATE=$(ETHOS_BAUDRATE) -DUSE_ETHOS_FOR_STDIO
  TERMDEPS += ethos
  TERMPROG ?= sudo $(RIOTTOOLS)/ethos/ethos
  TERMFLAGS ?= $(TAP) $(PORT) $(ETHOS_BAUDRATE)
endif
USEMODULE += auto_init_gnrc_netif

USEMODULE += shell
USEMODULE += shell_commands

USEMODULE += posix_inet

LOW_MEMORY_BOARDS := nucleo-f334r8 msb-430 msb-430h

ifeq ($(BOARD),$(filter $(BOARD),$(LOW_MEMORY_BOARDS)))
    CFLAGS += -DGNRC_PKTBUF_SIZE=512 -DGNRC_NETIF_IPV6_ADDRS_NUMOF=2 \
              -DGNRC_NETIF_IPV6_GROUPS_NUMOF=2 -DGNRC_IPV6_NIB_NUMOF=1 \
              -DNRC_IPV6_NIB_OFFL_NUMOF=1
endif

# TEST_ON_CI_WHITELIST += all

.PHONY: ethos

ethos:
	$(Q)env -u CC -u CFLAGS make -C $(RIOTTOOLS)/ethos

include $(RIOTBASE)/Makefile.include
