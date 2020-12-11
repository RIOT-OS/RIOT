BOARD ?= mcb2388

include ../Makefile.tests_common

BLOBS += hello.raw

USEMODULE += dac_dds
USEMODULE += shell

# Disable the greeting sample if the board has too little memory
# or flashing takes too long
ENABLE_GREETING ?= 1
CFLAGS += -DENABLE_GREETING=$(ENABLE_GREETING)

include $(RIOTBASE)/Makefile.include
