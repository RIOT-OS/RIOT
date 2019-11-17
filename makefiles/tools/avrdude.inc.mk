FLASHER = avrdude
DIST_PATH = $(RIOTBOARD)/$(BOARD)/dist
DEBUGSERVER_PORT = 4242
DEBUGSERVER = $(DIST_PATH)/debug_srv.sh
DEBUGSERVER_INTERFACE ?=
DEBUGSERVER_FLAGS = "-g -j usb $(DEBUGSERVER_INTERFACE) :$(DEBUGSERVER_PORT)"
DEBUGGER_FLAGS = "-x $(RIOTBOARD)/$(BOARD)/dist/gdb.conf $(ELFFILE)"
DEBUGGER = $(DIST_PATH)/debug.sh $(DEBUGSERVER_FLAGS) $(DIST_PATH) $(DEBUGSERVER_PORT)

PROGRAMMER_FLAGS = -p $(subst atmega,m,$(CPU))

# Set flasher port only for programmers that require it
ifneq (,$(filter $(PROGRAMMER),arduino avr109 buspirate stk500v1 stk500v2 wiring))
  # make the flasher port configurable (e.g. with atmelice the port is usb)
  # defaults to terminal's serial port if not configured
  PROGRAMMER_FLAGS += -P $(PROG_DEV)
endif
PROGRAMMER_FLAGS += $(FFLAGS_EXTRA)

# don't force to flash HEXFILE, but set it as default
FLASHFILE ?= $(HEXFILE)
FFLAGS += -c $(PROGRAMMER) $(PROGRAMMER_FLAGS) -U flash:w:$(FLASHFILE)
RESET ?= $(FLASHER) -c $(PROGRAMMER) $(PROGRAMMER_FLAGS)
