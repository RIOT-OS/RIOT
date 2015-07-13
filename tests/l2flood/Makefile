APPLICATION = l2flood

BOARD ?= native

RIOTBASE ?= $(CURDIR)/../..

USEMODULE += ng_netif_default
USEMODULE += auto_init_ng_netif
USEMODULE += ng_ipv6_default
USEMODULE += ng_udp
USEMODULE += ng_pktdump
USEMODULE += ng_icmpv6_echo
USEMODULE += uart0
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps

QUIET ?= 1

include $(RIOTBASE)/Makefile.include
