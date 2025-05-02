# Include common testing definitions
include ../Makefile.pkg_common

# This test depends on tap device setup not being set
PORT=

# Include packages that pull up and auto-init the link layer.
# NOTE: 6LoWPAN will be included if IEEE802.15.4 devices are present
USEMODULE += netdev_default
USEMODULE += auto_init_gnrc_netif
USEMODULE += gnrc_netif_single

# Additional networking modules that can be dropped if not needed
USEMODULE += netutils

# Specify the mandatory networking modules for IPv6 and UDP
USEMODULE += gnrc_ipv6_default

USEMODULE += memarray

# a cryptographically secure implementation of PRNG is needed for tinydtls
CFLAGS += -DWITH_RIOT_SOCK
CFLAGS += -DDTLS_PEER_MAX=2 -DDTLS_HANDSHAKE_MAX=2 -DNETQ_MAXCNT=5
USEPKG += tinydtls
USEMODULE += prng_sha1prng

# libcoap support
USEPKG += libcoap

USEMODULE += ztimer64_xtimer_compat

# libcoap needs some space
CFLAGS += -DTHREAD_STACKSIZE_MAIN=\(3*THREAD_STACKSIZE_DEFAULT\)

include $(RIOTBASE)/Makefile.include
