# name of your application
APPLICATION = huffman_ee_test
USEMODULE += huffman_ee

# compression packages that ship with RIOT, used for the comparison in the
# example
USEPKG += heatshrink
USEPKG += lz4
USEPKG += uzlib

# The example sends at most 14 different values. A small symbol bound keeps
# both the static tables and the stack needed by huffman_ee_build_exact()
# small, so the example fits the boards that build it.
CFLAGS += -DCONFIG_HUFFMAN_EE_MAX_SYMBOLS=32

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../../..

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

include $(RIOTBASE)/Makefile.include

