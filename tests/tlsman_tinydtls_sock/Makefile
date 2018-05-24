include ../Makefile.tests_common

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# Minimum setting
USEMODULE += gnrc_netdev_default
USEMODULE += auto_init_gnrc_netif
USEMODULE += gnrc_ipv6_default

USEMODULE += tlsman

# NOTE: Network stack must be defined by the user not by TLSMAN
USEMODULE += gnrc_sock_udp

# This is intended to be handled by sock_secure for normal applications
# NOTE (TODO): Equivalent to use `USEMODULE += tlsman_tinydtls`
USEPKG += tinydtls
ifneq (,$(filter tinydtls,$(USEPKG)))
	USEMODULE += tlsman_tinydtls

	CFLAGS += -DTHREAD_STACKSIZE_MAIN=\(3*THREAD_STACKSIZE_DEFAULT\)
	# NOTE: For "rehandhsake", tinydtls requires at least two peers.
	# With the default values, the boards will not crash but tinydtls will
	# be unable to generate the context.
	CFLAGS += -DDTLS_CONTEXT_MAX=2
	CFLAGS += -DDTLS_PEER_MAX=2
	CFLAGS += -DDTLS_HANDSHAKE_MAX=2
endif

# Special include directory with the (D)TLS keys required for the test
INCLUDES += -I$(APPDIR)/keys

# Default DTLS Server for the test
REM_SERVER ?='"fd00:dead:beef::1"'
CFLAGS += -DREMOTE_SERVER=$(REM_SERVER)

include $(RIOTBASE)/Makefile.include

test:
	./tests/01-run.py
