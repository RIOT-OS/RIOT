include ../Makefile.net_common

USEMODULE += netutils
USEMODULE += netif
USEMODULE += embunit
USEMODULE += sock_util

# make sure we have an implementation of sock_types.h
USEMODULE += sock_udp
USEMODULE += gnrc_ipv6

USEMODULE += ipv4_addr
USEMODULE += ipv6_addr

USEMODULE += sock_dns_mock

# We don't actually run the mock netifs
CFLAGS += -DGNRC_NETIF_MSG_QUEUE_SIZE=1

include $(RIOTBASE)/Makefile.include
