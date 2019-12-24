include ../Makefile.tests_common

# If no BOARD is found in the environment, use this default:
BOARD ?= native

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
