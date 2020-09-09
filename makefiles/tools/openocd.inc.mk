FLASHER ?= $(RIOTTOOLS)/openocd/openocd.sh
DEBUGGER = $(RIOTTOOLS)/openocd/openocd.sh
DEBUGSERVER = $(RIOTTOOLS)/openocd/openocd.sh
RESET ?= $(RIOTTOOLS)/openocd/openocd.sh

FLASHFILE ?= $(ELFFILE)
FFLAGS ?= flash $(FLASHFILE)
DEBUGGER_FLAGS ?= debug $(ELFFILE)
DEBUGSERVER_FLAGS ?= debug-server
RESET_FLAGS ?= reset

ifneq (,$(DEBUG_ADAPTER))
  include $(RIOTMAKE)/tools/openocd-adapters/$(DEBUG_ADAPTER).inc.mk
  OPENOCD_ADAPTER_INIT += -c 'transport select $(OPENOCD_TRANSPORT)'
endif

OPENOCD_CONFIG ?= $(BOARDDIR)/dist/openocd.cfg

# Export OPENOCD_CONFIG to required targets
OPENOCD_TARGETS = debug% flash% reset
$(call target-export-variables,$(OPENOCD_TARGETS),OPENOCD_CONFIG)

# Export OPENOCD_ADAPTER_INIT to required targets
$(call target-export-variables,$(OPENOCD_TARGETS),OPENOCD_ADAPTER_INIT)

# Export OPENOCD_RESET_USE_CONNECT_ASSERT_SRST to required targets
$(call target-export-variables,$(OPENOCD_TARGETS),OPENOCD_RESET_USE_CONNECT_ASSERT_SRST)

ifneq (,$(OPENOCD_CMD_RESET_RUN))
  # Export OPENOCD_CMD_RESET_RUN only to the reset target
  $(call target-export-variables,reset,OPENOCD_CMD_RESET_RUN)
endif

OPENOCD_FLASH_TARGETS = flash flash-only

ifneq (,$(OPENOCD_PRE_VERIFY_CMDS))
  # Export OPENOCD_PRE_VERIFY_CMDS only to the flash/flash-only target
  $(call target-export-variables,$(OPENOCD_FLASH_TARGETS),OPENOCD_PRE_VERIFY_CMDS)
endif

ifneq (,$(OPENOCD_PRE_FLASH_CMDS))
  # Export OPENOCD_PRE_FLASH_CMDS only to the flash/flash-only targets
  $(call target-export-variables,$(OPENOCD_FLASH_TARGETS),OPENOCD_PRE_FLASH_CMDS)
endif

ifneq (,$(PRE_FLASH_CHECK_SCRIPT))
  # Export PRE_FLASH_CHECK_SCRIPT only to the flash/flash-only targets
  $(call target-export-variables,$(OPENOCD_FLASH_TARGETS),PRE_FLASH_CHECK_SCRIPT)
endif
