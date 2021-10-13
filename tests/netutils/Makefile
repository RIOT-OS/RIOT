include ../Makefile.tests_common

USEMODULE += netutils
USEMODULE += netif
USEMODULE += embunit

# make sure we have an implementation of sock_types.h
USEMODULE += gnrc_sock_udp
USEMODULE += gnrc_ipv6

# pretend to include sock_dns
CFLAGS += -DMODULE_SOCK_DNS=1

include $(RIOTBASE)/Makefile.include
