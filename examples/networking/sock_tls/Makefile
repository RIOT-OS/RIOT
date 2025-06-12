APPLICATION = ttls_wolfssl

BOARD ?= native

RIOTBASE ?= $(CURDIR)/../RIOT

USEMODULE += sock_tls
USEMODULE += netdev_default
USEMODULE += auto_init_gnrc_netif

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
