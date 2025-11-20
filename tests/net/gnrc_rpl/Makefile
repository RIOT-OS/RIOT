include ../Makefile.net_common

USEMODULE += auto_init_gnrc_netif
USEMODULE += auto_init_gnrc_rpl
USEMODULE += gnrc_ipv6_router_default
USEMODULE += gnrc_icmpv6_echo
USEMODULE += gnrc_rpl

USEMODULE += shell
USEMODULE += shell_cmds_default

ifneq (,$(filter native native32 native64,$(BOARD)))
  USEMODULE += socket_zep
  USEMODULE += socket_zep_hello
  USEMODULE += netdev
  TERMFLAGS += -z 127.0.0.1:17754 # Murdock has no IPv6 support
else
  USEMODULE += netdev_default
  # automated test only works on native
  TESTS=
endif

.PHONY: host-tools

host-tools:
	$(Q)env -u CC -u CFLAGS $(MAKE) -C $(RIOTTOOLS)

TEST_DEPS += host-tools

# Test is flaky and regularly derails unrelated merge trains
TEST_ON_CI_BLACKLIST += native32 native64

include $(RIOTBASE)/Makefile.include
