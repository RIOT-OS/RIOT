# name of your application
APPLICATION = dtls_wolfssl

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

USEPKG += wolfssl
USEMODULE += wolfcrypt
USEMODULE += wolfssl
USEMODULE += wolfssl_dtls

# Select public key algorithm (or PSK) support fot ciphersuite(s):
#USEMODULE += wolfcrypt_ecc
#USEMODULE += wolfcrypt_rsa wolfcrypt_dh
USEMODULE += wolfssl_psk

# Uncomment the following line to add debug symbols
#CFLAGS+=-g -ggdb3

CFLAGS += -DDTLS_DEFAULT_PORT=$(DTLS_PORT) -DDTLS_WOLFSSL -Wno-unused-parameter -Wno-unused-variable

# A larger stack size is required if using ECC or RSA
CFLAGS += -DTHREAD_STACKSIZE_MAIN=\(3*THREAD_STACKSIZE_DEFAULT\)

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

include $(RIOTBASE)/Makefile.include
