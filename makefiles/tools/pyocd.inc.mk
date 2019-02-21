export FLASHER ?= $(RIOTBASE)/dist/tools/pyocd/pyocd.sh
export DEBUGGER = $(RIOTBASE)/dist/tools/pyocd/pyocd.sh
export DEBUGSERVER = $(RIOTBASE)/dist/tools/pyocd/pyocd.sh
export RESET ?= $(RIOTBASE)/dist/tools/pyocd/pyocd.sh

export OFLAGS ?= -O ihex
export FFLAGS ?= flash $(HEXFILE)
export DEBUGGER_FLAGS ?= debug $(ELFFILE)
export DEBUGSERVER_FLAGS ?= debug-server
export RESET_FLAGS ?= reset
