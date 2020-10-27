BOARD ?= mcb2388

include ../Makefile.tests_common

BLOBS += hello.raw

USEMODULE += dac_dds
USEMODULE += shell

LOW_MEMORY_BOARDS := samd10-xmini

# Disable the greeting sample if the board has too little memory
# or flashing takes too long
ifneq (,$(filter $(BOARD),$(LOW_MEMORY_BOARDS)))
  ENABLE_GREETING ?= 0
  CFLAGS += -DDAC_BUF_SIZE=1024
else
  ENABLE_GREETING ?= 1
endif

CFLAGS += -DENABLE_GREETING=$(ENABLE_GREETING)

include $(RIOTBASE)/Makefile.include
