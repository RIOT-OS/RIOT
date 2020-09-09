# set default port depending on operating system
PORT_LINUX ?= /dev/ttyACM0
PORT_DARWIN ?= $(firstword $(sort $(wildcard /dev/tty.usbmodem*)))
# Use DEBUG_ADAPTER_ID to specify the programmer serial number to use:
# DEBUG_ADAPTER_ID="ATML..."

# The SERIAL setting is only available for backwards compatibility with older
# settings. It's only checked when a target is given to the make command.
ifneq (,$(filter debug% flash% %term test,$(MAKECMDGOALS)))
  ifneq (,$(SERIAL))
    EDBG_ARGS += --serial $(SERIAL)
    SERIAL_TTY = $(firstword $(shell $(RIOTTOOLS)/usb-serial/find-tty.sh $(SERIAL)))
    ifeq (,$(SERIAL_TTY))
      $(error Did not find a device with serial $(SERIAL))
    endif
    PORT_LINUX := $(SERIAL_TTY)
    DEBUG_ADAPTER_ID ?= $(SERIAL)
  endif
endif

# setup serial terminal
include $(RIOTMAKE)/tools/serial.inc.mk

# Default for these boards is to use a CMSIS-DAP programmer
DEBUG_ADAPTER ?= dap

# If no programmer is set, select a default programmer
ifeq ($(PROGRAMMER),)
  # EDBG can only be used with a compatible Atmel programmer
  ifeq ($(DEBUG_ADAPTER),dap)
    # set this to either openocd, jlink or edbg
    PROGRAMMER ?= edbg
  else ifeq ($(DEBUG_ADAPTER),jlink)
    # only use JLinkExe if it's installed
    ifneq (,$(shell which JLinkExe))
      PROGRAMMER ?= jlink
    else
      PROGRAMMER ?= openocd
    endif
  else
    PROGRAMMER ?= openocd
  endif
endif

ifeq ($(PROGRAMMER),edbg)
  # use edbg for flashing
  include $(RIOTMAKE)/tools/edbg.inc.mk
  # use openocd for debugging
  include $(RIOTMAKE)/tools/openocd.inc.mk
else ifeq ($(PROGRAMMER),jlink)
  # this board uses J-Link for debug and possibly flashing
  include $(RIOTMAKE)/tools/jlink.inc.mk
else ifeq ($(PROGRAMMER),openocd)
  # this board uses openocd for debug and possibly flashing
  include $(RIOTMAKE)/tools/openocd.inc.mk
endif
