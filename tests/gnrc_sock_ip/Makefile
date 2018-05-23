include ../Makefile.tests_common

BOARD_INSUFFICIENT_MEMORY := chronos nucleo-f031k6 nucleo-f042k6 nucleo-l031k6

USEMODULE += gnrc_sock_ip
USEMODULE += gnrc_ipv6
USEMODULE += ps

CFLAGS += -DGNRC_PKTBUF_SIZE=200
CFLAGS += -DTEST_SUITES

TEST_ON_CI_WHITELIST += all

include $(RIOTBASE)/Makefile.include

test:
	./tests/01-run.py
