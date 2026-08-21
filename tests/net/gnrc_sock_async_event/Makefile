include ../Makefile.net_common

USEMODULE += auto_init_gnrc_netif
USEMODULE += gnrc_ipv6_hdr
USEMODULE += gnrc_pktdump
USEMODULE += sock_async_event
USEMODULE += sock_ip
USEMODULE += sock_udp
USEMODULE += od
USEMODULE += xtimer

CFLAGS += -DTEST_SUITES

include $(RIOTBASE)/Makefile.include

# Set GNRC_PKTBUF_SIZE via CFLAGS if not being set via Kconfig.
ifndef CONFIG_GNRC_PKTBUF_SIZE
  CFLAGS += -DCONFIG_GNRC_PKTBUF_SIZE=208
endif
