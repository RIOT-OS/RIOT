# name of your application
include ../Makefile.net_common

# fmt module for scn_buf_hex() function
USEMODULE += fmt
# load shell modules
USEMODULE += shell
USEMODULE += shell_cmds_default
USEMODULE += shell_cmd_gnrc_pktbuf

# ztimer for time-sensitive tasks
USEMODULE += ztimer_msec

# use IEEE 802.15.4 as link-layer protocol
USEMODULE += netdev_ieee802154
USEMODULE += netdev_test
# 6LoWPAN and its extensions
USEMODULE += gnrc_sixlowpan_default
# IPv6 host support with extension headers
USEMODULE += gnrc_ipv6_default
USEMODULE += gnrc_ipv6_ext
# UDP
USEMODULE += gnrc_udp
# Dumps packets
USEMODULE += gnrc_pktdump

include $(RIOTBASE)/Makefile.include
