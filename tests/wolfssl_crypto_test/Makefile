APPLICATION = wolf_crypto_test

# If no BOARD is found in the environment, use this default:
#BOARD ?= native
#BOARD ?= samd21-xpro #Currently missing implementation for strncasecmp
#BOARD ?= pic32-wifire
#BOARD ?= frdm-k64f
BOARD ?= stm32f4discovery

# BOARD_WHITELIST := To Be Determined
# BOARD_BLACKLIST := To Be Determined
# BOARD_INSUFFICIENT_MEMORY := To Be Determined

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

# Used for inserting debug symbols into library for portability
# testing on Ubuntu 16.04 LTS w/ gcc v 5.4.0
#CFLAGS += -g
#LDFLAGS += -g

# Necessary to set the stacksize on Ubuntu Native device when using
# gcc v 5.4.0 on 16.04 LTS
# This is an optimized stack value based on testing, if you observe
# a segmentation fault please increase this stack size.
CFLAGS += -DTHREAD_STACKSIZE_MAIN=35000

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

# May need these for seeding wolfCrypt DRBG
#USEMODULE += prng_minstd

USEPKG += wolfssl
USEMODULE += wolfcrypt wolfcrypt-test wolfcrypt_sha512 wolfcrypt_curve25519  \
			 wolfcrypt_ed25519 wolfcrypt_chacha wolfcrypt_poly1305 wolfcrypt_aes \
			 wolfcrypt_ecc wolfcrypt_asn wolfcrypt_random

ifneq (native, $(BOARD))
# for TCP/IP recommend working with lwip or other third-party TCP/IP stack.
# Also need third-party string library if attempting to use ecc that will
# implement strncasecomp
#    USEPKG += lwip
#    export CFLAGS += "-DHAVE_LWIP_NATIVE=1"
endif

include $(RIOTBASE)/Makefile.include
