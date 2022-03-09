# name of your application
include ../Makefile.tests_common

# use IEEE 802.15.4 as link-layer protocol
USEMODULE += netdev_ieee802154
USEMODULE += netdev_test
# 6LoWPAN and its extensions
USEMODULE += gnrc_sixlowpan_default
# IPv6 host support
USEMODULE += gnrc_ipv6_default
# UDP
USEMODULE += gnrc_udp
# Dumps packets
USEMODULE += gnrc_pktdump

include $(RIOTBASE)/Makefile.include
