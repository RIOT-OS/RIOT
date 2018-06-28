# set default port depending on operating system
PORT_LINUX ?= /dev/ttyACM0
PORT_DARWIN ?= $(firstword $(sort $(wildcard /dev/tty.usbmodem*)))
# Use DEBUG_ADAPTER_ID to specify the programmer serial number to use:
# export DEBUG_ADAPTER_ID="ATML..."

# The SERIAL setting is only available for backwards compatibility with older
# settings.
ifneq (,$(SERIAL))
  EDBG_ARGS += --serial $(SERIAL)
  SERIAL_TTY = $(firstword $(shell $(RIOTTOOLS)/usb-serial/find-tty.sh $(SERIAL)))
  ifeq (,$(SERIAL_TTY))
    $(error Did not find a device with serial $(SERIAL))
  endif
  PORT_LINUX := $(SERIAL_TTY)
  export DEBUG_ADAPTER_ID ?= $(SERIAL)
endif

# setup serial terminal
include $(RIOTMAKE)/tools/serial.inc.mk

# Default for these boards is to use a CMSIS-DAP programmer
export DEBUG_ADAPTER ?= dap

# EDBG can only be used with a compatible Atmel programmer
ifeq ($(DEBUG_ADAPTER),dap)
  # set this to either openocd or edbg
  PROGRAMMER ?= edbg
else
  PROGRAMMER ?= openocd
endif

# use edbg if selected and a device type has been set
ifeq ($(PROGRAMMER),edbg)
  ifneq (,$(EDBG_DEVICE_TYPE))
    include $(RIOTMAKE)/tools/edbg.inc.mk
  endif
endif

# this board uses openocd for debug and possibly flashing
include $(RIOTMAKE)/tools/openocd.inc.mk
