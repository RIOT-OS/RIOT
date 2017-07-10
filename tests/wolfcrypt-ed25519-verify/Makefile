# name of your application
APPLICATION = wolfcrypt-ed25519-verify

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

CFLAGS+=-DNO_ED25519_SIGN -DNO_ED25519_KEY_EXPORT

USEPKG +=wolfssl
USEMODULE += wolfssl
USEMODULE += wolfcrypt
USEMODULE += wolfcrypt_ed25519

include $(RIOTBASE)/Makefile.include
