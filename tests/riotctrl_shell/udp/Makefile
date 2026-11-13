USEMODULE += shell_cmd_udp
USEMODULE += shell_cmd_udp_server

USEMODULE += fmt
USEMODULE += ipv6_addr
USEMODULE += od
USEMODULE += shell_cmd_sys
USEMODULE += sock_async_event
USEMODULE += sock_udp
USEMODULE += ztimer_usec

include $(CURDIR)/../Makefile.riotctrl_shell_common

INCLUDES += -I$(CURDIR)
CFLAGS += -DSOCK_HAS_ASYNC
CFLAGS += -DSOCK_HAS_IPV6
CFLAGS += -DTESTING_MOCK_SOCK_UDP
