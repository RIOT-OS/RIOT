include ../Makefile.tests_common

# TinyDTLS only has support for 32-bit architectures ATM
FEATURES_REQUIRED += arch_32bit

# Include packages that pull up and auto-init the link layer.
# NOTE: 6LoWPAN will be included if IEEE802.15.4 devices are present
USEMODULE += netdev_default
USEMODULE += auto_init_gnrc_netif
# Specify the mandatory networking modules for IPv6 and UDP
USEMODULE += gnrc_ipv6_default
USEMODULE += sock_async_event
USEMODULE += sock_dtls
USEMODULE += sock_udp
USEMODULE += sock_util
USEMODULE += event_thread_medium
USEMODULE += event_timeout

# Use tinydtls for sock_dtls
USEPKG += tinydtls
# tinydtls needs crypto secure PRNG
USEMODULE += prng_sha1prng

# Add also the shell, some shell commands
USEMODULE += shell
USEMODULE += shell_commands

# Bitfield to log open sessions
USEMODULE += bitfield

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
CFLAGS += -DEVENT_THREAD_MEDIUM_STACKSIZE=\(2*THREAD_STACKSIZE_LARGE\)

include $(RIOTBASE)/Makefile.include
