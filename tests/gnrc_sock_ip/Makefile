include ../Makefile.tests_common

USEMODULE += gnrc_sock_ip
USEMODULE += gnrc_ipv6
USEMODULE += ps

CFLAGS += -DCONFIG_GNRC_PKTBUF_SIZE=200
CFLAGS += -DTEST_SUITES

include $(RIOTBASE)/Makefile.include
