include ../Makefile.tests_common

USEMODULE += netutils
USEMODULE += netif
USEMODULE += embunit
USEMODULE += sock_util

# make sure we have an implementation of sock_types.h
USEMODULE += gnrc_sock_udp
USEMODULE += gnrc_ipv6

USEMODULE += ipv4_addr
USEMODULE += ipv6_addr

USEMODULE += sock_dns_mock

include $(RIOTBASE)/Makefile.include
