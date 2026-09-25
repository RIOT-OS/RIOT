APPLICATION = ttls_wolfssl

BOARD ?= native

RIOTBASE ?= $(CURDIR)/../../..

USEMODULE += sock_tls_tcp
USEMODULE += netdev_default
USEMODULE += auto_init_gnrc_netif
USEMODULE += gnrc_ipv6_default
USEMODULE += gnrc_icmpv6_echo
USEMODULE += gnrc_sock_tcp

# WolfSSL modules
USEPKG += wolfssl
USEMODULE += wolfssl
USEMODULE += wolfssl_tls
USEMODULE += wolfcrypt
USEMODULE += wolfcrypt_aes
USEMODULE += wolfcrypt_sha
USEMODULE += wolfcrypt_hmac
USEMODULE += wolfcrypt_random
USEMODULE += wolfssl_psk

# Shell and utils
USEMODULE += shell
USEMODULE += shell_cmds_default
USEMODULE += ps

include $(RIOTBASE)/Makefile.include
