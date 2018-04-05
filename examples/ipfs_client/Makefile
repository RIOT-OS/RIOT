# name of your application
APPLICATION = ipfs_client

# If no BOARD is found in the environment, use this default:
BOARD ?= iotlab-m3

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

BOARD_PROVIDES_NETIF := airfy-beacon cc2538dk fox iotlab-m3 iotlab-a8-m3 mulle \
        microbit native nrf51dongle nrf52dk nrf6310 openmote-cc2538 pba-d-01-kw2x \
        remote-pa remote-reva samr21-xpro \
        spark-core telosb yunjia-nrf51822 z1

# Modules to include:
USEMODULE += saul_default
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps

USEMODULE += gnrc_netdev_default
USEMODULE += auto_init_gnrc_netif
USEMODULE += gnrc_ipv6_router_default

USEMODULE += gnrc_tcp
USEMODULE += gnrc_udp
USEMODULE += gnrc_sock
USEMODULE += gnrc_icmpv6_echo
USEMODULE += gnrc_txtsnd
USEMODULE += gnrc_pktdump
USEMODULE += gnrc_rpl
USEMODULE += auto_init_gnrc_rpl

CFLAGS += -DGNRC_PKTBUF_SIZE=512
CFLAGS += -DDEVELHELP -DHTTPDEBUG
CFLAGS += -Wno-implicit-fallthrough -Wno-unused-parameter -Wno-sign-compare

USEMODULE += netstats_l2
USEMODULE += netstats_ipv6
USEMODULE += netstats_rpl

FEATURES_OPTIONAL += periph_rtc

include $(RIOTBASE)/Makefile.include

# Set a custom channel if needed
ifneq (,$(filter cc110x,$(USEMODULE)))          # radio is cc110x sub-GHz
  DEFAULT_CHANNEL ?= 0
  CFLAGS += -DCC110X_DEFAULT_CHANNEL=$(DEFAULT_CHANNEL)
else
  ifneq (,$(filter at86rf212b,$(USEMODULE)))    # radio is IEEE 802.15.4 sub-GHz
    DEFAULT_CHANNEL ?= 5
    CFLAGS += -DIEEE802154_DEFAULT_SUBGHZ_CHANNEL=$(DEFAULT_CHANNEL)
  else                                          # radio is IEEE 802.15.4 2.4 GHz
    DEFAULT_CHANNEL ?= 26
    CFLAGS += -DIEEE802154_DEFAULT_CHANNEL=$(DEFAULT_CHANNEL)
  endif
endif
