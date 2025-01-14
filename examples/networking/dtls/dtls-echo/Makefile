# name of your application
APPLICATION = dtls_echo

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../../../..

# Include packages that pull up and auto-init the link layer.
# NOTE: 6LoWPAN will be included if IEEE802.15.4 devices are present
USEMODULE += netdev_default
USEMODULE += auto_init_gnrc_netif
# Specify the mandatory networking modules for IPv6 and UDP
USEMODULE += gnrc_ipv6_default
USEMODULE += sock_udp

# Add also the shell, some shell commands
USEMODULE += shell
USEMODULE += shell_cmds_default

USEPKG += tinydtls
# tinydtls needs crypto secure PRNG
USEMODULE += prng_sha1prng

# UDP Port to use (20220 is default for DTLS).
DTLS_PORT ?= 20220
CFLAGS += -DDTLS_DEFAULT_PORT=$(DTLS_PORT)

# NOTE: If no cipher suite is selected, CONFIG_DTLS_PSK is used by default.
# This section should be commented out if using Kconfig
# This adds support for TLS_PSK_WITH_AES_128_CCM_8
# CFLAGS += -DCONFIG_DTLS_PSK
# This adds support for TLS_ECDHE_ECDSA_WITH_AES_128_CCM_8
# CFLAGS += -DCONFIG_DTLS_ECC

# Enable this line for setting tinyDTLS in debug mode.
# CFLAGS += -DCONFIG_DTLS_DEBUG

# FIXME: This is a temporary patch
# TinyDTLS <= 0.8.6 requires around 426 bytes in RAM.
CFLAGS += -DTHREAD_STACKSIZE_MAIN=\(3*THREAD_STACKSIZE_DEFAULT\)
# TINYDTLS_EXTRA_BUFF can be used for increasing the server stack memory.
# CFLAGS += -DTINYDTLS_EXTRA_BUFF=\(512\)

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

include $(RIOTBASE)/Makefile.include
