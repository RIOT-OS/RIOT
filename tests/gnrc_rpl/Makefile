include ../Makefile.tests_common

USEMODULE += auto_init_gnrc_netif
USEMODULE += auto_init_gnrc_rpl
USEMODULE += gnrc_ipv6_router_default
USEMODULE += gnrc_icmpv6_echo
USEMODULE += gnrc_rpl

USEMODULE += shell
USEMODULE += shell_commands

# automated test only works on native
TEST_ON_CI_WHITELIST += native

ifeq (native, $(BOARD))
  USEMODULE += socket_zep
  USEMODULE += socket_zep_hello
  USEMODULE += netdev
  TERMFLAGS += -z 127.0.0.1:17754 # Murdock has no IPv6 support
else
  USEMODULE += netdev_default
endif

.PHONY: host-tools

host-tools:
	$(Q)env -u CC -u CFLAGS $(MAKE) -C $(RIOTTOOLS)

TERMDEPS += host-tools

include $(RIOTBASE)/Makefile.include
