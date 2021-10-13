# Bus Pirate debug adapter
#
# For SWD mode connect:
#  MOSI - SWDIO
#   CLK - SWCLK

ifeq ($(OS),Darwin)
  PROG_DEV ?= $(firstword $(sort $(wildcard /dev/tty.usbmodem*)))
else
  PROG_DEV ?= /dev/ttyUSB0
endif

OPENOCD_ADAPTER_INIT ?= -c 'source [find interface/buspirate.cfg]' \
                        -c 'buspirate_port $(PROG_DEV)'
