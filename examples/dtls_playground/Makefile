# name of your application
export APPLICATION = dtls_playground

# If no BOARD is found in the environment, use this default:
export BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
export RIOTBASE ?= $(CURDIR)/../..

# Change this to 0 show compiler invocation lines by default:
export QUIET ?= 1

USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += posix
USEMODULE += pnet
USEMODULE += ps
USEMODULE += vtimer
USEMODULE += defaulttransceiver
USEMODULE += sixlowpan
USEMODULE += config
USEMODULE += destiny
USEMODULE += dtls
USEMODULE += aes
USEMODULE += crypto_ciphers
USEMODULE += crypto_modes

include $(RIOTBASE)/Makefile.include
