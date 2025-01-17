BOARD ?= nrf52840dk
include ../Makefile.sys_common

# this needs to be outsourced to the respective makefiles
USEMODULE += nfc_nfct
USEMODULE += nfc_ndef
USEMODULE += ztimer_sec
USEMODULE += ztimer
USEMODULE += event

CFLAGS += -DISR_STACKSIZE=1024*2
CFLAGS += -DTHREAD_STACKSIZE_DEFAULT=1024*2
CFLAGS += -Wno-error=cast-align
CFLAGS += -Wno-error=unused-variable
CFLAGS += -DLOG_LEVEL=LOG_DEBUG



include $(RIOTBASE)/Makefile.include
