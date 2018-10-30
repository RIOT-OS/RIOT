DEVELHELP := 1
# name of your application
include ../Makefile.tests_common

BOARD_INSUFFICIENT_MEMORY := arduino-duemilanove arduino-mega2560 arduino-uno \
                             hifive1 mega-xplained msb-430 msb-430h \
                             nucleo-f030r8 nucleo-f031k6 nucleo-f042k6 \
                             nucleo-f070rb nucleo-f072rb nucleo-f303k8 \
                             nucleo-f334r8 nucleo-l031k6 nucleo-l053r8 \
                             stm32f0discovery telosb thingy52 waspmote-pro \
                             wsn430-v1_3b wsn430-v1_4 z1
# chronos, mips-malta, and ruuvitag boards don't support ethos
BOARD_BLACKLIST := chronos mips-malta ruuvitag

export TAP ?= tap0

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
# Specify the mandatory networking modules for IPv6
USEMODULE += gnrc_ipv6_router_default
USEMODULE += gnrc_icmpv6_error
USEMODULE += gnrc_pktdump
USEMODULE += gnrc_pktbuf_cmd
# IPv6 extension headers
USEMODULE += gnrc_ipv6_ext
USEMODULE += od
# Add also the shell, some shell commands
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps

# TEST_ON_CI_WHITELIST += all

.PHONY: ethos

ethos:
	$(Q)env -u CC -u CFLAGS make -C $(RIOTTOOLS)/ethos

include $(RIOTBASE)/Makefile.include
