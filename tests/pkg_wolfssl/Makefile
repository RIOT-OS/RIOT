include ../Makefile.tests_common

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# Only 32-bit targets
BOARD_BLACKLIST := arduino-duemilanove arduino-leonardo \
                   arduino-mega2560 arduino-nano arduino-uno \
                   atmega328p chronos esp8266-esp-12x esp8266-olimex-mod \
                   esp8266-sparkfun-thing mega-xplained microduino-corerf \
                   msb-430 msb-430h telosb waspmote-pro \
                   wsn430-v1_3b wsn430-v1_4 z1

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

# This is an optimized stack value based on testing, if you observe
# a segmentation fault please increase this stack size.
CFLAGS += -DTHREAD_STACKSIZE_MAIN=2*THREAD_STACKSIZE_LARGE

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1


USEPKG += wolfssl
USEMODULE += wolfcrypt wolfcrypt-test wolfcrypt_sha512 wolfcrypt_curve25519  \
			 wolfcrypt_ed25519 wolfcrypt_chacha wolfcrypt_poly1305 wolfcrypt_aes \
			 wolfcrypt_ecc wolfcrypt_asn wolfcrypt_random

# Uncomment the following line to enable RSA tests
# (e.g. when enough resources are available on platform)
#USEMODULE += wolfcrypt_rsa wolfcrypt_dh

# Comment the following line to disable full-benchmark test
USEMODULE += wolfcrypt-benchmark

ifneq ($(BOARD),native)
  CFLAGS += -DBENCH_EMBEDDED
endif

include $(RIOTBASE)/Makefile.include
