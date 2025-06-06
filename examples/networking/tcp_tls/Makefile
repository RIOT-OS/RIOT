APPLICATION = ttls_wolfssl

BOARD ?= native

RIOTBASE ?= $(CURDIR)/../RIOT

USEMODULE += tcp_tls
USEMODULE += netdev_default
USEMODULE += auto_init_gnrc_netif
USEMODULE += gnrc_ipv6_default
USEMODULE += gnrc_icmpv6_echo
USEMODULE += gnrc_sock_tcp
USEMODULE += sock_tcp
USEMODULE += sock_udp

# WolfSSL modules
USEPKG += wolfssl
USEMODULE += wolfssl
USEMODULE += wolfssl_tls
USEMODULE += wolfcrypt
USEMODULE += wolfcrypt_aes
USEMODULE += wolfcrypt_rsa
USEMODULE += wolfcrypt_dh
USEMODULE += wolfcrypt_ecc
USEMODULE += wolfcrypt_sha
USEMODULE += wolfcrypt_md5
USEMODULE += wolfcrypt_hmac
USEMODULE += wolfcrypt_random
USEMODULE += wolfssl_dtls
USEMODULE += wolfssl_psk

# Shell and utils
USEMODULE += shell
USEMODULE += shell_cmds_default
USEMODULE += ps

SRC += main.c

# Network flags
CFLAGS += -DGNRC_TCP

# Debug flags
CFLAGS += -DDEBUG_WOLFSSL
CFLAGS += -DGNRC_TCP
CFLAGS += -DDTLS_DEFAULT_PORT=$(DTLS_PORT) -DDTLS_WOLFSSL -Wno-unused-parameter -Wno-unused-variable

include $(RIOTBASE)/Makefile.include
