export FLASHER = $(RIOTTOOLS)/jlink/jlink.sh
export DEBUGGER = $(RIOTTOOLS)/jlink/jlink.sh
export DEBUGSERVER = $(RIOTTOOLS)/jlink/jlink.sh
export RESET = $(RIOTTOOLS)/jlink/jlink.sh

FLASHFILE ?= $(BINFILE)

export FFLAGS ?= flash $(FLASHFILE)
export DEBUGGER_FLAGS ?= debug $(ELFFILE)
export DEBUGSERVER_FLAGS ?= debug-server
export RESET_FLAGS ?= reset
