include ../Makefile.tests_common

USEMODULE += auto_init_gnrc_netif
USEMODULE += gnrc_ipv6_default
USEMODULE += gnrc_neterr
USEMODULE += gnrc_sock_udp
USEMODULE += od
USEMODULE += xtimer

CFLAGS += -DCONFIG_GNRC_PKTBUF_SIZE=200

include $(RIOTBASE)/Makefile.include
