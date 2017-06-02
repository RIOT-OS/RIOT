# set default port depending on operating system
PORT_LINUX ?= /dev/ttyACM0
PORT_DARWIN ?= $(firstword $(sort $(wildcard /dev/tty.usbmodem*)))

# setup serial terminal
include $(RIOTMAKE)/tools/serial.inc.mk

# Add board selector (USB serial) to OpenOCD options if specified.
# Use /dist/tools/usb-serial/list-ttys.sh to find out serial number.
#   Usage: SERIAL="ATML..." BOARD=<board> make flash
ifneq (,$(SERIAL))
    export OPENOCD_EXTRA_INIT += "-c cmsis_dap_serial $(SERIAL)"
    EDBG_ARGS += "--serial $(SERIAL)"
    SERIAL_TTY = $(firstword $(shell $(RIOTBASE)/dist/tools/usb-serial/find-tty.sh $(SERIAL)))
    ifeq (,$(SERIAL_TTY))
        $(error Did not find a device with serial $(SERIAL))
    endif
    PORT_LINUX := $(SERIAL_TTY)
endif

# set this to either openocd or edbg
PROGRAMMER ?= edbg

# use edbg if selected and a device type has been set
ifeq ($(PROGRAMMER),edbg)
  ifneq (,$(EDBG_DEVICE_TYPE))
    include $(RIOTMAKE)/tools/edbg.inc.mk
  endif
endif

# this board uses openocd for debug and possibly flashing
include $(RIOTMAKE)/tools/openocd.inc.mk
