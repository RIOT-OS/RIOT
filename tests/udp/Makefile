APPLICATION = udp
include ../Makefile.tests_common

USEMODULE += embunit
USEMODULE += gnrc_ipv6
USEMODULE += gnrc_nettest
USEMODULE += gnrc_udp

DISABLE_MODULE = auto_init

CFLAGS += -DTEST_SUITES

include $(RIOTBASE)/Makefile.include

test:
	./tests/01-run.py
