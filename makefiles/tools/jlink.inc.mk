FLASHER ?= $(RIOTTOOLS)/jlink/jlink.sh
DEBUGGER ?= $(RIOTTOOLS)/jlink/jlink.sh
DEBUGSERVER ?= $(RIOTTOOLS)/jlink/jlink.sh
RESET ?= $(RIOTTOOLS)/jlink/jlink.sh

FLASHFILE ?= $(EXECUTABLE)

FFLAGS ?= flash $(FLASHFILE)
DEBUGGER_FLAGS ?= debug $(DEBUG_EXECUTABLE)
DEBUGSERVER_FLAGS ?= debug-server
RESET_FLAGS ?= reset

JLINK_SERIAL ?= $(DEBUG_ADAPTER_ID)

JLINK_IF ?=
JLINK_RESET_FILE ?=
JLINK_PRE_FLASH ?=
JLINK_POST_FLASH ?=

JLINK_FLASH_TARGETS = flash flash%
JLINK_TARGETS = debug% $(JLINK_FLASH_TARGETS) reset term-rtt

# Export JLINK_SERIAL to required targets
$(call target-export-variables,$(JLINK_TARGETS),JLINK_SERIAL)

# Export JLINK_DEVICE to required targets
$(call target-export-variables,$(JLINK_TARGETS),JLINK_DEVICE)

ifneq (,$(JLINK))
  # Export JLINK to required targets if not empty
  $(call target-export-variables,$(JLINK_TARGETS),JLINK)
endif

ifneq (,$(JLINK_IF))
  # Export JLINK_IF to required targets if not empty
  $(call target-export-variables,$(JLINK_TARGETS),JLINK_IF)
endif

ifneq (,$(JLINK_RESET_FILE))
  # Export JLINK_RESET_FILE to required targets if not empty
  $(call target-export-variables,$(JLINK_TARGETS),JLINK_RESET_FILE)
endif

# Export JLINK_PRE_FLASH to flash targets only if not empty
ifneq (,$(JLINK_PRE_FLASH))
  $(call target-export-variables,JLINK_FLASH_TARGETS,JLINK_PRE_FLASH)
endif

# Export JLINK_POST_FLASH to flash targets only if not empty
ifneq (,$(JLINK_POST_FLASH))
  $(call target-export-variables,JLINK_FLASH_TARGETS,JLINK_POST_FLASH)
endif
