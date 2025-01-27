BOARD ?= nrf52840dk
include ../Makefile.net_common

USEMODULE += nfc_nfct
USEMODULE += ztimer_sec
USEMODULE += ztimer
USEMODULE += event

CFLAGS += -DDEVELHELP=1
CFLAGS += -DISR_STACKSIZE=1024*2
CFLAGS += -DTHREAD_STACKSIZE_DEFAULT=1024*2
CFLAGS += -Wno-error=cast-align
CFLAGS += -Wno-error=unused-variable
CFLAGS += -DLOG_LEVEL=LOG_DEBUG



include $(RIOTBASE)/Makefile.include
