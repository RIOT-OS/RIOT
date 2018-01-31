# convenience makefile exporting the commonly used /dev/ttyACM0 based serial
# configuration
PORT_LINUX ?= /dev/ttyACM0
PORT_DARWIN ?= $(firstword $(sort $(wildcard /dev/tty.usbmodem*)))

include $(RIOTMAKE)/tools/serial.inc.mk
