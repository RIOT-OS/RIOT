BOARD ?= nrf52840dk
include ../Makefile.pkg_common

USEPKG += nrfx

# include nrfx_nfct support
USEMODULE += nrfx_nfct

USEMODULE += ztimer
USEMODULE += ztimer_sec
USEMODULE += ztimer_msec
USEMODULE += event_queue

CFLAGS += -Wno-error=cast-align
CFLAGS += -Wno-error=unused-variable
CFLAGS += -Wno-error=unused-parameter
CFLAGS += -Wno-error=unused-function
CFLAGS += -DDEBUG_ASSERT_VERBOSE
CFLAGS += -DDEVELHELP
CFLAGS += -DLOG_LEVEL=LOG_DEBUG
CFLAGS += -DISR_STACKSIZE=1024*2

include $(RIOTBASE)/Makefile.include