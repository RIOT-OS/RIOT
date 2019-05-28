FLASHER ?= $(RIOTBASE)/dist/tools/pyocd/pyocd.sh
DEBUGGER = $(RIOTBASE)/dist/tools/pyocd/pyocd.sh
DEBUGSERVER = $(RIOTBASE)/dist/tools/pyocd/pyocd.sh
RESET ?= $(RIOTBASE)/dist/tools/pyocd/pyocd.sh

export OFLAGS ?= -O ihex
FFLAGS ?= flash $(HEXFILE)
DEBUGGER_FLAGS ?= debug $(ELFFILE)
DEBUGSERVER_FLAGS ?= debug-server
RESET_FLAGS ?= reset
