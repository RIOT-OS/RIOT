include ../Makefile.tests_common

USEMODULE += sock_ip
USEMODULE += gnrc_ipv6
USEMODULE += ps

CFLAGS += -DTEST_SUITES

include $(RIOTBASE)/Makefile.include

# Set GNRC_PKTBUF_SIZE via CFLAGS if not being set via Kconfig.
ifndef CONFIG_GNRC_PKTBUF_SIZE
  CFLAGS += -DCONFIG_GNRC_PKTBUF_SIZE=200
endif
