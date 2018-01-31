# convenience makefile exporting the commonly used /dev/ttyUSB0 based serial
# configuration
PORT_LINUX ?= /dev/ttyUSB0
PORT_DARWIN ?= $(firstword $(sort $(wildcard /dev/tty.SLAB_USBtoUART*)))

include $(RIOTMAKE)/tools/serial.inc.mk
