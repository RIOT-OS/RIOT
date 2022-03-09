include ../Makefile.tests_common

USEMODULE += gnrc_ipv6_router_default
USEMODULE += gnrc_sixlowpan_frag_sfr
USEMODULE += gnrc_sixlowpan_iphc
USEMODULE += gnrc_ipv6_nib
USEMODULE += gnrc_netif
USEMODULE += embunit
USEMODULE += netdev_ieee802154
USEMODULE += netdev_test

CFLAGS += -DTEST_SUITES

include $(RIOTBASE)/Makefile.include

ifndef CONFIG_GNRC_IPV6_NIB_NO_RTR_SOL
  # disable router solicitations so they don't interfere with the tests
  CFLAGS += -DCONFIG_GNRC_IPV6_NIB_NO_RTR_SOL=1
endif
# SFR parameters
ifndef CONFIG_GNRC_SIXLOWPAN_SFR_OPT_WIN_SIZE
  # fix window size
  CFLAGS += -DCONFIG_GNRC_SIXLOWPAN_SFR_OPT_WIN_SIZE=3U
endif
ifndef CONFIG_GNRC_SIXLOWPAN_SFR_INTER_FRAME_GAP_US
  # decrease inter frame gap
  CFLAGS += -DCONFIG_GNRC_SIXLOWPAN_SFR_INTER_FRAME_GAP_US=5U
endif
ifndef CONFIG_GNRC_SIXLOWPAN_SFR_MIN_ARQ_TIMEOUT_MS
  # decrease minimal ARQ timeout
  CFLAGS += -DCONFIG_GNRC_SIXLOWPAN_SFR_MIN_ARQ_TIMEOUT_MS=100U
endif
ifndef CONFIG_GNRC_SIXLOWPAN_SFR_OPT_ARQ_TIMEOUT_MS
  # decrease initial ARQ timeout
  CFLAGS += -DCONFIG_GNRC_SIXLOWPAN_SFR_OPT_ARQ_TIMEOUT_MS=100U
endif
