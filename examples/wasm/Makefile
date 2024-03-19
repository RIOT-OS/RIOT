APPLICATION = wasm-example
# If no BOARD is defined in the environment, use this default:
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

USEPKG += wamr

ifeq (,$(filter native native64,$(BOARD)))
CFLAGS += -DTHREAD_STACKSIZE_MAIN='(6 * 1024)'
endif

export WAMR_CONFIG := $(abspath config.cmake)

BLOBS += test.wasm hello.wasm

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

all: hello.wasm

hello.wasm: wasm_sample/hello.c wasm_sample/Makefile
	make -C wasm_sample hello.wasm
	mv wasm_sample/hello.wasm .

#######################################################
# Load the rest of the usual RIOT make infrastructure #
#######################################################

include $(RIOTBASE)/Makefile.include
