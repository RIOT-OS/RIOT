FLASHER ?= $(RIOTBASE)/dist/tools/pyocd/pyocd.sh
DEBUGGER = $(RIOTBASE)/dist/tools/pyocd/pyocd.sh
DEBUGSERVER = $(RIOTBASE)/dist/tools/pyocd/pyocd.sh
RESET ?= $(RIOTBASE)/dist/tools/pyocd/pyocd.sh

FLASHFILE ?= $(HEXFILE)
FFLAGS ?= flash $(FLASHFILE)
DEBUGGER_FLAGS ?= debug $(ELFFILE)
DEBUGSERVER_FLAGS ?= debug-server
RESET_FLAGS ?= reset
