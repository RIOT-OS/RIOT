APPLICATION = gnrc_sock_udp
include ../Makefile.tests_common

RIOTBASE ?= $(CURDIR)/../..

BOARD_INSUFFICIENT_MEMORY := nucleo32-f042 nucleo32-f031

USEMODULE += gnrc_sock_check_reuse
USEMODULE += gnrc_sock_udp
USEMODULE += gnrc_ipv6
USEMODULE += ps

CFLAGS += -DDEVELHELP
CFLAGS += -DGNRC_PKTBUF_SIZE=400
CFLAGS += -DTEST_SUITES

include $(RIOTBASE)/Makefile.include

test:
	./tests/01-run.py
