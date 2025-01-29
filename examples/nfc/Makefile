APPLICATION = NFC_SAMPLE
BOARD ?= nrf52840dk
RIOTBASE ?= $(CURDIR)/../..
LTO=1

USEMODULE += nfc_nfct
USEMODULE += ztimer_sec
USEMODULE += ztimer_msec
USEMODULE += ztimer
USEMODULE += event
DEVELHELP = 1

# include SAUL to interact with onboard sensors and actuators
USEMODULE += saul

# include and auto-initialize all available sensors
USEMODULE += saul_default

CFLAGS += -DISR_STACKSIZE=1024*2
CFLAGS += -DTHREAD_STACKSIZE_DEFAULT=1024*2
CFLAGS += -Wno-error=cast-align
CFLAGS += -Wno-error=unused-variable
CFLAGS += -DLOG_LEVEL=LOG_DEBUG
CFLAGS += -DNRFX_NFCT_T2T_EMULATOR_TIMING=1


include $(RIOTBASE)/Makefile.include
