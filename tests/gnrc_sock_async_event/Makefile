include ../Makefile.tests_common

USEMODULE += auto_init_gnrc_netif
USEMODULE += gnrc_ipv6_hdr
USEMODULE += gnrc_pktdump
USEMODULE += sock_async_event
USEMODULE += sock_ip
USEMODULE += sock_udp
USEMODULE += od
USEMODULE += xtimer

# mock IPv6 gnrc_nettype
CFLAGS += -DTEST_SUITES -DGNRC_NETTYPE_IPV6=GNRC_NETTYPE_TEST

include $(RIOTBASE)/Makefile.include

# Set GNRC_PKTBUF_SIZE via CFLAGS if not being set via Kconfig.
ifndef CONFIG_GNRC_PKTBUF_SIZE
  CFLAGS += -DCONFIG_GNRC_PKTBUF_SIZE=200
endif
