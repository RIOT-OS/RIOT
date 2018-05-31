include ../Makefile.tests_common

BOARD_INSUFFICIENT_MEMORY := calliope-mini chronos hifive1 microbit msb-430 msb-430h \
                             nucleo-f031k6 nucleo-f042k6 nucleo-f303k8 nucleo-l031k6 \
                             nucleo-f030r8 nucleo-f070rb nucleo-f072rb nucleo-f103rb nucleo-f302r8 \
                             nucleo-f334r8 nucleo-l053r8 spark-core stm32f0discovery telosb \
                             wsn430-v1_3b wsn430-v1_4 z1

USEMODULE += gnrc_netdev_default
USEMODULE += auto_init_gnrc_netif
USEMODULE += gnrc_ipv6_router_default
USEMODULE += gnrc_udp
USEMODULE += gnrc_rpl
USEMODULE += auto_init_gnrc_rpl
USEMODULE += od
USEMODULE += gnrc_icmpv6_echo
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps
USEMODULE += netstats_l2
USEMODULE += netstats_ipv6

include $(RIOTBASE)/Makefile.include

# Set a custom channel if needed
ifneq (,$(filter cc110x,$(USEMODULE)))          # radio is cc110x sub-GHz
  DEFAULT_CHANNEL ?= 0
  CFLAGS += -DCC110X_DEFAULT_CHANNEL=$(DEFAULT_CHANNEL)
else
  ifneq (,$(filter at86rf212b,$(USEMODULE)))    # radio is IEEE 802.15.4 sub-GHz
    DEFAULT_CHANNEL ?= 5
    FLAGS += -DIEEE802154_DEFAULT_SUBGHZ_CHANNEL=$(DEFAULT_CHANNEL)
  else                                          # radio is IEEE 802.15.4 2.4 GHz
    DEFAULT_CHANNEL ?= 26
    CFLAGS += -DIEEE802154_DEFAULT_CHANNEL=$(DEFAULT_CHANNEL)
  endif
endif
