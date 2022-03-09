BOARD ?= nucleo-f767zi
include ../Makefile.tests_common

# Enable hardware timestamping, if possible
FEATURES_OPTIONAL += periph_ptp periph_eth

USEMODULE += auto_init_gnrc_netif
USEMODULE += fmt
USEMODULE += gnrc_icmpv6_echo
USEMODULE += gnrc_ipv6_default
USEMODULE += netdev_default
USEMODULE += gnrc_sock_udp
USEMODULE += netstats_ipv6
USEMODULE += netstats_l2
USEMODULE += ps
USEMODULE += shell
USEMODULE += shell_commands

USEMODULE += sock_aux_local
USEMODULE += sock_aux_rssi
USEMODULE += sock_aux_timestamp

include $(RIOTBASE)/Makefile.include
