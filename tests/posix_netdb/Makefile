include ../Makefile.tests_common

USEMODULE += embunit

# make sure we have an implementation of sock_types.h
USEMODULE += sock_udp
USEMODULE += gnrc_ipv6

USEMODULE += ipv4_addr
USEMODULE += ipv6_addr

# use mock data for the test
USEMODULE += sock_dns_mock

USEMODULE += posix_netdb

include $(RIOTBASE)/Makefile.include
