# name of your application
APPLICATION = riot-wrappers-test-networkproperties
BOARD ?= native
APPLICATION_RUST_MODULE = riot_wrappers_test_networkproperties

# This may not be a GNRC netdev in all cases; when it is not, the test will
# break, and that will be the time to split it. So far, also IPv6 support can
# just be assumed. (Really, anyone writing an IoT application without IPv6
# support may want to look into Cobol).
USEMODULE += netdev_default
USEMODULE += auto_init_gnrc_netif
USEMODULE += gnrc_ipv6_default
# This is an easy way to visibly populate entries into the neighbor cache: ping
# the RIOT instance.
USEMODULE += gnrc_icmpv6_echo

USEMODULE += ztimer_msec

include $(RIOTBASE)/Makefile.include
