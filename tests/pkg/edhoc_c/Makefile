include ../Makefile.pkg_common

FEATURES_REQUIRED += netif
# Cannot run the test on `murdock` in `native`
#   open(/dev/net/tun): No such file or directory
TEST_ON_CI_BLACKLIST += native32 native64

# Test fails pretty regularly on CI
TEST_ON_CI_BLACKLIST += esp32-wroom-32

# Edhoc related packages
USEPKG += edhoc-c
USEMODULE += edhoc-c_crypto_tinycrypt
# USEMODULE += edhoc-c_crypto_wolfssl
USEMODULE += edhoc-c_cbor_nanocbor

# Include packages that pull up and auto-init the link layer.
# NOTE: 6LoWPAN will be included if IEEE802.15.4 devices are present
USEMODULE += netdev_default

USEMODULE += auto_init_gnrc_netif
# Specify the mandatory networking modules for IPv6 and UDP
USEMODULE += gnrc_ipv6_router_default
USEMODULE += sock_udp
# Additional networking modules that can be dropped if not needed
USEMODULE += gnrc_icmpv6_echo
USEMODULE += nanocoap_sock

# include this for printing IP addresses
USEMODULE += shell_cmds_default
USEMODULE += ps
USEMODULE += xtimer

# This is an optimized stack value based on testing, if you observe
# a segmentation fault please increase this stack size.
CFLAGS += -DTHREAD_STACKSIZE_MAIN=3*THREAD_STACKSIZE_LARGE
CFLAGS += -DCONFIG_NANOCOAP_SERVER_STACK_SIZE=3*THREAD_STACKSIZE_LARGE

# Include responder code
CONFIG_INITIATOR ?= 1
CFLAGS += -DCONFIG_INITIATOR=$(CONFIG_INITIATOR)
# Include responder code
CONFIG_RESPONDER ?= 1
CFLAGS += -DCONFIG_RESPONDER=$(CONFIG_RESPONDER)


ifeq (1, $(CONFIG_RESPONDER))
  USEMODULE += nanocoap_server_auto_init
  CFLAGS += -DCONFIG_NANOCOAP_SERVER_BUF_SIZE=512
endif

include $(RIOTBASE)/Makefile.include
include $(RIOTMAKE)/default-radio-settings.inc.mk
