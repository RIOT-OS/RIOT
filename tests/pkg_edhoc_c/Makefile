include ../Makefile.tests_common

include $(RIOTBASE)/tests/Makefile.boards.netif
BOARD_WHITELIST += $(BOARD_PROVIDES_NETIF)
# Cannot run the test on `murdock` in `native`
#   open(/dev/net/tun): No such file or directory
TEST_ON_CI_BLACKLIST += native

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
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps
USEMODULE += xtimer

# This is an optimized stack value based on testing, if you observe
# a segmentation fault please increase this stack size.
CFLAGS += -DTHREAD_STACKSIZE_MAIN=3*THREAD_STACKSIZE_LARGE

# Include responder code
CONFIG_INITIATOR ?= 1
CFLAGS += -DCONFIG_INITIATOR=$(CONFIG_INITIATOR)
# Include responder code
CONFIG_RESPONDER ?= 1
CFLAGS += -DCONFIG_RESPONDER=$(CONFIG_RESPONDER)

include $(RIOTBASE)/Makefile.include
include $(RIOTMAKE)/default-radio-settings.inc.mk
