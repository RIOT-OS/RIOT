# name of your application
APPLICATION = dtls_sock

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

# Include packages that pull up and auto-init the link layer.
# NOTE: 6LoWPAN will be included if IEEE802.15.4 devices are present
USEMODULE += netdev_default
USEMODULE += auto_init_gnrc_netif
# Specify the mandatory networking modules for IPv6 and UDP
USEMODULE += gnrc_ipv6_default
USEMODULE += netutils

# Specify DTLS implementation
USEPKG += tinydtls

# Pull in sock APIs
USEMODULE += sock_dtls
# When using asymmetric cryptography, e.g. ecc,
# this verifies the public key against a known set.
# It does not affect symmetric crypto e.g. PSK.
USEMODULE += sock_dtls_verify_public_key
USEMODULE += sock_udp

# extra utilities for endpoint printing
USEMODULE += sock_util

# tinydtls needs crypto secure PRNG
USEMODULE += prng_sha1prng

# Add also the shell, some shell commands
USEMODULE += shell
USEMODULE += shell_cmds_default

# UDP Port to use (20220 is default for DTLS).
DTLS_PORT ?= 20220
CFLAGS += -DDTLS_DEFAULT_PORT=$(DTLS_PORT)

# NOTE: If no cipher suite is selected, CONFIG_DTLS_PSK is used by default.
# This section should be commented out if using Kconfig
# This adds support for TLS_PSK_WITH_AES_128_CCM_8
# CFLAGS += -DCONFIG_DTLS_PSK
# This adds support for TLS_ECDHE_ECDSA_WITH_AES_128_CCM_8
# CFLAGS += -DCONFIG_DTLS_ECC

# Uncomment to enable debug logs
# CFLAGS += -DCONFIG_DTLS_DEBUG

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1
# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

include $(RIOTBASE)/Makefile.include

# FIXME: This is a temporary patch
ifeq (,$(filter arch_avr8,$(FEATURES_USED)))
  CFLAGS += -DTHREAD_STACKSIZE_MAIN=\(2*THREAD_STACKSIZE_LARGE\)
else
  CFLAGS += -DTHREAD_STACKSIZE_MAIN=\(4*THREAD_STACKSIZE_LARGE\)
endif

# We have two credentials for client and server, each. If we are to run both
# server and client, we need four slots in total. The default (2) would be
# sufficient if only client or server is ever run.
CFLAGS += -DCONFIG_CREDMAN_MAX_CREDENTIALS=4
