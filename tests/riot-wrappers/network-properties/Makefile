include ../Makefile.riot-wrappers_common

# name of your application
APPLICATION = riot-wrappers-test-networkproperties
BOARD ?= native
APPLICATION_RUST_MODULE = riot_wrappers_test_networkproperties

# This may not be a GNRC netdev in all cases; when it is not, the test will
# break, and that will be the time to split it. So far, also IPv6 support can
# just be assumed. (Really, anyone writing an IoT application without IPv6
# support may want to look into Cobol).

ifneq (,$(filter native native32 native64,$(BOARD)))
  # Use ZEP instead of a TAP interface, which is not available in CI
  USEMODULE += socket_zep
  USEMODULE += netdev
  USEMODULE += gnrc_netif_ieee802154
  TERMFLAGS ?= -z "localhost:17754"
endif

USEMODULE += netdev_default
USEMODULE += auto_init_gnrc_netif
USEMODULE += gnrc_ipv6_default
# This is an easy way to visibly populate entries into the neighbor cache: ping
# the RIOT instance.
USEMODULE += gnrc_icmpv6_echo

USEMODULE += ztimer_msec

include $(RIOTBASE)/Makefile.include
