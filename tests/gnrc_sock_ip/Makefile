APPLICATION = gnrc_sock_ip
include ../Makefile.tests_common

BOARD_INSUFFICIENT_MEMORY := nucleo32-f031

RIOTBASE ?= $(CURDIR)/../..

USEMODULE += gnrc_sock_ip
USEMODULE += gnrc_ipv6
USEMODULE += ps

CFLAGS += -DDEVELHELP
CFLAGS += -DGNRC_PKTBUF_SIZE=200
CFLAGS += -DTEST_SUITES

include $(RIOTBASE)/Makefile.include

test:
	./tests/01-run.py
