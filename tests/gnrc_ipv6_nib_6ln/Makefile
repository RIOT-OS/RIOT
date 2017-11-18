# name of your application
APPLICATION = gnrc_ipv6_nib_6ln
include ../Makefile.tests_common

BOARD_INSUFFICIENT_MEMORY := chronos nucleo-f030 nucleo-l053 nucleo32-f031 \
                             nucleo32-l031 nucleo32-f042 stm32f0discovery

USEMODULE += gnrc_ipv6
USEMODULE += gnrc_sixlowpan
USEMODULE += gnrc_ipv6_nib_6ln
USEMODULE += embunit
USEMODULE += netopt

CFLAGS += -DDEVELHELP
CFLAGS += -DGNRC_NETTYPE_NDP2=GNRC_NETTYPE_TEST
CFLAGS += -DGNRC_PKTBUF_SIZE=512
CFLAGS += -DTEST_SUITES

include $(RIOTBASE)/Makefile.include

test:
	tests/01-run.py
