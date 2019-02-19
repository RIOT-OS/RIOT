FLASHER = avrdude
DIST_PATH = $(RIOTBOARD)/$(BOARD)/dist
DEBUGSERVER_PORT = 4242
DEBUGSERVER = $(DIST_PATH)/debug_srv.sh
DEBUGSERVER_FLAGS = "-g -j usb :$(DEBUGSERVER_PORT)"
DEBUGGER_FLAGS = "-x $(RIOTBOARD)/$(BOARD)/dist/gdb.conf $(ELFFILE)"
DEBUGGER = $(DIST_PATH)/debug.sh $(DEBUGSERVER_FLAGS) $(DIST_PATH) $(DEBUGSERVER_PORT)

# make the flasher port configurable (e.g. with atmelice the port is usb)
# defaults to terminal's serial port if not configured
AVRDUDE_PORT    ?= $(PORT)
PROGRAMMER_FLAGS = -P $(AVRDUDE_PORT) $(FFLAGS_EXTRA)

# don't force to flash HEXFILE, but set it as default
FLASHFILE ?= $(HEXFILE)
FFLAGS += -c $(PROGRAMMER) $(PROGRAMMER_FLAGS) -U flash:w:$(HEXFILE)
