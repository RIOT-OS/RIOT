# name of your application
APPLICATION = crypto-benchmark

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

# Include packages
USEMODULE += crypto
USEMODULE += hashes

# Required features
FEATURES_REQUIRED = periph_rtt

# Optional features
FEATURES_OPTIONAL = periph_hwcrypto

# By defining CRYPTO_THREEDES, cipher_context_t has biggest context of the
# supported software ciphers.
CFLAGS += -DCRYPTO_THREEDES

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
CFLAGS += -DDEVELHELP

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

include $(RIOTBASE)/Makefile.include
