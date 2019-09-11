include ../Makefile.tests_common

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# Only 32-bit targets
BOARD_BLACKLIST := arduino-duemilanove arduino-leonardo \
                   arduino-mega2560 arduino-nano \
                   arduino-uno chronos esp8266-esp-12x esp8266-olimex-mod \
                   esp8266-sparkfun-thing jiminy-mega256rfr2 mega-xplained \
                   msb-430 msb-430h telosb waspmote-pro \
                   wsn430-v1_3b wsn430-v1_4 z1

#
# If the test fails to run on a target, disable the algorithms
# that you do not plan to use, by removing the associated wolfcrypt_* modules
# from USEMODULE list below.
#
# You may be able to remove your target from the list below by selecting a
# smaller set of modules.
#
# Targets not included in the BOARD_INSUFFICIENT_MEMORY are capable of
# running all the benchmarks selected by default.
#
BOARD_INSUFFICIENT_MEMORY := nucleo-f042k6 stm32f0discovery nucleo-f334r8 \
                             i-nucleo-lrwan1 nucleo-f302r8 nucleo-l053r8 saml11-xpro \
                             nucleo-l031k6 bluepill stm32l0538-disco saml10-xpro \
                             nucleo-f030r8 nucleo-f031k6 nucleo-f303k8 blackpill

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
