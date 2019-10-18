include ../Makefile.tests_common

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# Only 32-bit targets
BOARD_BLACKLIST := arduino-duemilanove arduino-leonardo \
                   arduino-mega2560 arduino-nano arduino-uno atmega328p \
                   chronos esp8266-esp-12x esp8266-olimex-mod \
                   esp8266-sparkfun-thing mega-xplained microduino-corerf \
                   msb-430 msb-430h telosb waspmote-pro \
                   wsn430-v1_3b wsn430-v1_4 z1

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

# Ensure there is enough stack space
CFLAGS += -DTHREAD_STACKSIZE_MAIN=THREAD_STACKSIZE_LARGE

CFLAGS+=-DNO_ED25519_SIGN -DNO_ED25519_KEY_EXPORT

USEPKG +=wolfssl
USEMODULE += wolfcrypt
USEMODULE += wolfcrypt_ed25519

include $(RIOTBASE)/Makefile.include
