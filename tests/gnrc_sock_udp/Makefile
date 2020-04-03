include ../Makefile.tests_common

USEMODULE += gnrc_sock_check_reuse
USEMODULE += gnrc_sock_udp
USEMODULE += gnrc_ipv6
USEMODULE += ps

CFLAGS += -DGNRC_PKTBUF_SIZE=400
CFLAGS += -DTEST_SUITES

include $(RIOTBASE)/Makefile.include
