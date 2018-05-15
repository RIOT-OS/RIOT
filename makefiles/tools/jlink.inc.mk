export FLASHER = $(RIOTTOOLS)/jlink/jlink.sh
export DEBUGGER = $(RIOTTOOLS)/jlink/jlink.sh
export DEBUGSERVER = $(RIOTTOOLS)/jlink/jlink.sh
export RESET = $(RIOTTOOLS)/jlink/jlink.sh

HEXFILE = $(BINFILE)

export FFLAGS ?= flash
export DEBUGGER_FLAGS ?= debug
export DEBUGSERVER_FLAGS ?= debug-server
export RESET_FLAGS ?= reset
