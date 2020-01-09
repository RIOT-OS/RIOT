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
USEMODULE += gnrc_sock_udp

# Use tinydtls for sock_dtls
USEMODULE += tinydtls_sock_dtls

# Add also the shell, some shell commands
USEMODULE += shell
USEMODULE += shell_commands

# UDP Port to use (20220 is default for DTLS).
DTLS_PORT ?= 20220
CFLAGS += -DDTLS_DEFAULT_PORT=$(DTLS_PORT)

# NOTE: If no cipher suite is selected, DTLS_PSK is used by default.
# This adds support for TLS_PSK_WITH_AES_128_CCM_8
CFLAGS += -DDTLS_PSK
# This adds support for TLS_ECDHE_ECDSA_WITH_AES_128_CCM_8
# CFLAGS += -DDTLS_ECC

# Uncomment to enable debug logs
# CFLAGS += -DDTLS_DEBUG
# When DTLS_DEBUG is set and using tinydtls, verbosity of debug log can be set with
# Values: 0:EMERG (Default), 1:ALERT 2:CRIT 3:WARN 4:NOTICE 5:INFO 6:DEBUG
# TINYDTLS_LOG=3

# FIXME: This is a temporary patch
CFLAGS += -DTHREAD_STACKSIZE_MAIN=\(2*THREAD_STACKSIZE_LARGE\)

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1
# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

include $(RIOTBASE)/Makefile.include
