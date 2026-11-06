USEMODULE += cord_ep_standalone
USEMODULE += shell_cmd_cord_ep

USEMODULE += fmt
USEMODULE += ipv6_addr
USEMODULE += od
USEMODULE += shell_cmd_sys
USEMODULE += sock_async_event
USEMODULE += sock_udp

include $(CURDIR)/../Makefile.riotctrl_shell_common

INCLUDES += -I$(CURDIR)
CFLAGS += -DSOCK_HAS_ASYNC
CFLAGS += -DSOCK_HAS_IPV6
CFLAGS += -DTESTING_MOCK_SOCK_UDP
