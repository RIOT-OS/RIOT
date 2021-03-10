# name of your application
APPLICATION = dtls_sock

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

# TinyDTLS only has support for 32-bit architectures ATM
FEATURES_REQUIRED += arch_32bit

# Include packages that pull up and auto-init the link layer.
# NOTE: 6LoWPAN will be included if IEEE802.15.4 devices are present
USEMODULE += gnrc_netdev_default
USEMODULE += auto_init_gnrc_netif
# Specify the mandatory networking modules for IPv6 and UDP
USEMODULE += gnrc_ipv6_default

# Specify DTLS implementation
USEPKG += tinydtls

# Pull in sock APIs
USEMODULE += sock_dtls
USEMODULE += sock_udp

# extra utilities for endpoint printing
USEMODULE += sock_util

# tinydtls needs crypto secure PRNG
USEMODULE += prng_sha1prng

# Add also the shell, some shell commands
USEMODULE += shell
USEMODULE += shell_commands

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

# FIXME: This is a temporary patch
CFLAGS += -DTHREAD_STACKSIZE_MAIN=\(2*THREAD_STACKSIZE_LARGE\)

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1
# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

include $(RIOTBASE)/Makefile.include
