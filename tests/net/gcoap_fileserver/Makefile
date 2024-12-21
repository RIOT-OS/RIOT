include ../Makefile.net_common

USEMODULE += auto_init_gnrc_netif
USEMODULE += auto_init_gnrc_rpl
USEMODULE += gnrc_ipv6_router_default
USEMODULE += gnrc_icmpv6_echo
USEMODULE += gnrc_rpl

USEMODULE += shell
USEMODULE += shell_cmds_default

USEMODULE += gcoap_fileserver
USEMODULE += nanocoap_fileserver_put

USEMODULE += nanocoap_vfs

USEMODULE += constfs
USEMODULE += vfs_default
USEMODULE += vfs_auto_format

USEMODULE += hashes
USEMODULE += shell_cmd_md5sum

# This integration test uncovers a bug somwehere in our stack, but there
# are currently no resources to debug it, so blacklist it to keep CI running
TEST_ON_CI_BLACKLIST += all

# automated test only works on native
# TEST_ON_CI_WHITELIST += native native32

# use small blocksize for test to increase chance for errors
CFLAGS += -DCONFIG_NANOCOAP_BLOCKSIZE_DEFAULT=COAP_BLOCKSIZE_16
CFLAGS += -DCONFIG_COAP_ACK_TIMEOUT_MS=100
CFLAGS += -DCONFIG_COAP_MAX_RETRANSMIT=10

ifneq (,$(filter native native32,$(BOARD)))
  USEMODULE += socket_zep
  USEMODULE += socket_zep_hello
  USEMODULE += netdev
  TERMFLAGS += -z 127.0.0.1:17754 # Murdock has no IPv6 support
  # make sure each instance gets their own fs
  CFLAGS += -DCONFIG_NATIVE_ISOLATE_FS=1
else
  USEMODULE += netdev_default
endif

.PHONY: host-tools

# Build zep_dispatch
host-tools:
	$(Q)env -u CC -u CFLAGS $(MAKE) -C $(RIOTTOOLS)

TEST_DEPS += host-tools

include $(RIOTBASE)/Makefile.include
