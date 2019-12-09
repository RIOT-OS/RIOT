include ../Makefile.tests_common

USEMODULE += auto_init_gnrc_netif
USEMODULE += gnrc_ipv6_hdr
USEMODULE += gnrc_pktdump
USEMODULE += gnrc_sock_async
USEMODULE += gnrc_sock_ip
USEMODULE += gnrc_sock_udp
USEMODULE += sock_async_event
USEMODULE += od
USEMODULE += xtimer

CFLAGS += -DSOCK_HAS_IPV6 -DGNRC_PKTBUF_SIZE=200
# mock IPv6 gnrc_nettype
CFLAGS += -DTEST_SUITES -DGNRC_NETTYPE_IPV6=GNRC_NETTYPE_TEST

include $(RIOTBASE)/Makefile.include
