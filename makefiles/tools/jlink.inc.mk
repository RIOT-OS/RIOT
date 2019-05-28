FLASHER = $(RIOTTOOLS)/jlink/jlink.sh
DEBUGGER = $(RIOTTOOLS)/jlink/jlink.sh
DEBUGSERVER = $(RIOTTOOLS)/jlink/jlink.sh
RESET = $(RIOTTOOLS)/jlink/jlink.sh

FLASHFILE ?= $(BINFILE)

FFLAGS ?= flash $(FLASHFILE)
DEBUGGER_FLAGS ?= debug $(ELFFILE)
DEBUGSERVER_FLAGS ?= debug-server
RESET_FLAGS ?= reset
