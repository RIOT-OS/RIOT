include ../Makefile.tests_common

AUX_LOCAL ?= 1

ifeq (1, $(AUX_LOCAL))
  USEMODULE += sock_aux_local
endif

USEMODULE += gnrc_sock_check_reuse
USEMODULE += sock_udp
USEMODULE += gnrc_ipv6
USEMODULE += ps

CFLAGS += -DTEST_SUITES

include $(RIOTBASE)/Makefile.include

# Set GNRC_PKTBUF_SIZE via CFLAGS if not being set via Kconfig.
ifndef CONFIG_GNRC_PKTBUF_SIZE
  CFLAGS += -DCONFIG_GNRC_PKTBUF_SIZE=400
endif
