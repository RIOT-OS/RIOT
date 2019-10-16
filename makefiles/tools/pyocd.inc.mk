FLASHER ?= $(RIOTBASE)/dist/tools/pyocd/pyocd.sh
DEBUGGER = $(RIOTBASE)/dist/tools/pyocd/pyocd.sh
DEBUGSERVER = $(RIOTBASE)/dist/tools/pyocd/pyocd.sh
RESET ?= $(RIOTBASE)/dist/tools/pyocd/pyocd.sh

FLASHFILE ?= $(HEXFILE)
FFLAGS ?= flash $(FLASHFILE)
DEBUGGER_FLAGS ?= debug $(ELFFILE)
DEBUGSERVER_FLAGS ?= debug-server
RESET_FLAGS ?= reset

# Add serial matching command, only if DEBUG_ADAPTER_ID was specified
ifneq (,$(DEBUG_ADAPTER_ID))
  export PYOCD_ADAPTER_INIT += -u $(DEBUG_ADAPTER_ID)
endif
