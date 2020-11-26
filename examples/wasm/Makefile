APPLICATION = wasm-example
# If no BOARD is defined in the environment, use this default:
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

USEPKG += wamr

USEMODULE += xtimer
USEMODULE += sema

export WAMR_CONFIG := $(abspath config.cmake)

WPEDANTIC := 0
WERROR := 0

BLOBS += main.wasm

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

######################################################
# Load the rest of the usual RIOT make infastructure #
######################################################

include $(RIOTBASE)/Makefile.include
