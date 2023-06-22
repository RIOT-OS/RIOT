RIOTBASE ?= $(CURDIR)/../../..
BOARD ?= atmega8
PORT ?= /dev/ttyACM0

include ../../Makefile.tests_common

USEMODULE += pcf8574a
EXTERNAL_MODULE_DIRS += ./
USEMODULE += hd44780_avr

CFLAGS += -DHD44780_PARAM_COLS=20
CFLAGS += -DHD44780_PARAM_ROWS=4
CFLAGS += -DPCF8574A_BASE_ADDR=0x3F

include $(RIOTBASE)/Makefile.include
