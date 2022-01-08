FLASHER ?= $(RIOTBASE)/dist/tools/pyocd/pyocd.sh
DEBUGGER ?= $(RIOTBASE)/dist/tools/pyocd/pyocd.sh
DEBUGSERVER ?= $(RIOTBASE)/dist/tools/pyocd/pyocd.sh
RESET ?= $(RIOTBASE)/dist/tools/pyocd/pyocd.sh

PYOCD_FLASH_TARGET_TYPE ?=
FLASHFILE ?= $(HEXFILE)
FFLAGS ?= flash $(FLASHFILE)
DEBUGGER_FLAGS ?= debug $(ELFFILE)
DEBUGSERVER_FLAGS ?= debug-server
RESET_FLAGS ?= reset

PYOCD_TARGETS = debug% flash% reset

# Export PYOCD_FLASH_TARGET_TYPE to required targets only if not empty
ifneq (,$(PYOCD_FLASH_TARGET_TYPE))
  $(call target-export-variables,$(PYOCD_TARGETS),PYOCD_FLASH_TARGET_TYPE)
endif

# Add serial matching command, only if DEBUG_ADAPTER_ID was specified
ifneq (,$(DEBUG_ADAPTER_ID))
  PYOCD_ADAPTER_INIT += -u $(DEBUG_ADAPTER_ID)
  $(call target-export-variables,$(PYOCD_TARGETS),PYOCD_ADAPTER_INIT)
endif
