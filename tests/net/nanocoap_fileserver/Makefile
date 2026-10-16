include ../Makefile.net_common

# nanocoap needs the sock types, the network is not used by the test
USEMODULE += gnrc_ipv6_default
USEMODULE += gnrc_sock_udp

USEMODULE += embunit
USEMODULE += nanocoap
USEMODULE += nanocoap_fileserver
USEMODULE += nanocoap_fileserver_put
USEMODULE += nanocoap_fileserver_delete
USEMODULE += nanocoap_fileserver_callback
USEMODULE += vfs_default
USEMODULE += vfs_util

# needs a writable file system, only the native file system provides one by default
FEATURES_REQUIRED += arch_native

# use a small block size to test blockwise transfers with little data
CFLAGS += -DCONFIG_NANOCOAP_BLOCK_SIZE_MAX=COAP_BLOCKSIZE_32

include $(RIOTBASE)/Makefile.include
