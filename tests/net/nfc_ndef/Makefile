BOARD ?= native
include ../Makefile.net_common

USEMODULE += nfc_ndef
USEMODULE += embunit

CFLAGS += -DLOG_LEVEL=LOG_DEBUG

# increase the stack size
CFLAGS += -DTHREAD_STACKSIZE_MAIN=4096

include $(RIOTBASE)/Makefile.include
