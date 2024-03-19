BOARD ?= stm32f429i-disc1
include ../Makefile.drivers_common

DISABLE_MODULE += test_utils_interactive_sync

USEMODULE += touch_dev
USEMODULE += ztimer
USEMODULE += ztimer_msec

TOUCH_DEV_POLLING_MODE ?= 0
TOUCH_DEV_POLLING_PERIOD ?= 50

CFLAGS += -DTOUCH_DEV_POLLING_MODE=$(TOUCH_DEV_POLLING_MODE)
CFLAGS += -DTOUCH_DEV_POLLING_PERIOD=$(TOUCH_DEV_POLLING_PERIOD)

include $(RIOTBASE)/Makefile.include
