APPLICATION = ipv6
#include ../Makefile.tests_common
BOARD ?= native
RIOTBASE ?= $(CURDIR)/../../../../../../../../
QUIET ?= 1

USEMODULE += embunit

USEMODULE += gnrc_ipv6

USEMODULE += gnrc_nettest

DISABLE_MODULE = auto_init

CFLAGS += -DTEST_SUITES

#include $(RIOTBASE)/Makefile.base
include $(RIOTBASE)/Makefile.include
