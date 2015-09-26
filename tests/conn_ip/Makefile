APPLICATION = conn_ip

BOARD ?= native

RIOTBASE ?= $(CURDIR)/../..

BOARD_INSUFFICIENT_MEMORY := chronos msb-430 msb-430h nucleo-f334 stm32f0discovery telosb \
                             wsn430-v1_3b wsn430-v1_4 z1

USEMODULE += gnrc_netif_default
USEMODULE += auto_init_gnrc_netif
USEMODULE += gnrc_icmpv6_echo
USEMODULE += gnrc_ipv6_default
USEMODULE += gnrc_conn_ip
USEMODULE += od
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps

QUIET ?= 1

include $(RIOTBASE)/Makefile.include

test:
	./tests/01-run.py
