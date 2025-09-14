FLASHER ?= $(RIOTTOOLS)/openocd/openocd.sh
DEBUGGER ?= $(RIOTTOOLS)/openocd/openocd.sh
DEBUGSERVER ?= $(RIOTTOOLS)/openocd/openocd.sh
DEBUGCLIENT ?= $(RIOTTOOLS)/openocd/openocd.sh
RESET ?= $(RIOTTOOLS)/openocd/openocd.sh

FLASHFILE ?= $(ELFFILE)
FFLAGS ?= flash $(FLASHFILE)
DEBUGGER_FLAGS ?= debug $(DEBUG_ELFFILE)
DEBUGSERVER_FLAGS ?= debug-server
DEBUGCLIENT_FLAGS ?= debug-client $(DEBUG_ELFFILE)
RESET_FLAGS ?= reset

ifneq (,$(OPENOCD_DEBUG_ADAPTER))
  include $(RIOTMAKE)/tools/openocd-adapters/$(OPENOCD_DEBUG_ADAPTER).inc.mk
  ifneq (default,$(OPENOCD_TRANSPORT))
    OPENOCD_ADAPTER_INIT += -c 'transport select $(OPENOCD_TRANSPORT)'
  endif
endif

# Use the board's custom OpenOCD by default, if present in the file system.
OPENOCD_CONFIG ?= $(wildcard $(BOARDDIR)/dist/openocd.cfg)

# Still no config?
ifeq (,$(OPENOCD_CONFIG))
  # MCU is STM32 based?
  ifeq (stm32,$(CPU))
    # Then use the common OpenOCD config for the STM32 family in use
    OPENOCD_CONFIG := $(RIOTBASE)/boards/common/stm32/dist/stm32$(CPU_FAM).cfg
  else
    $(warning "Even though PROGRAMMER is openocd, no OPENOCD_CONFIG is specified. Flashing and debugging won't work.")
  endif
endif

OPENOCD_TARGETS = debug% flash% reset

# Export GDB_PORT_CORE_OFFSET to required targets
$(call target-export-variables,$(OPENOCD_TARGETS),GDB_PORT_CORE_OFFSET)

# Export OPENOCD_CONFIG to required targets
$(call target-export-variables,$(OPENOCD_TARGETS),OPENOCD_CONFIG)

# Export OPENOCD_CORE to required targets
$(call target-export-variables,$(OPENOCD_TARGETS),OPENOCD_CORE)

# Export OPENOCD_ADAPTER_INIT to required targets
$(call target-export-variables,$(OPENOCD_TARGETS),OPENOCD_ADAPTER_INIT)

# Export OPENOCD_EXTRA_INIT to required targets
$(call target-export-variables,$(OPENOCD_TARGETS),OPENOCD_EXTRA_INIT)

# Export OPENOCD_EXTRA_RESET_INIT to required targets
$(call target-export-variables,$(OPENOCD_TARGETS),OPENOCD_EXTRA_RESET_INIT)

# Export OPENOCD_RESET_USE_CONNECT_ASSERT_SRST to required targets
$(call target-export-variables,$(OPENOCD_TARGETS),OPENOCD_RESET_USE_CONNECT_ASSERT_SRST)

ifneq (,$(OPENOCD_SERVER_ADDRESS))
# Export OPENOCD_SERVER_ADDRESS to required targets
  $(call target-export-variables,$(OPENOCD_TARGETS),OPENOCD_SERVER_ADDRESS)
endif

ifneq (,$(OPENOCD_CMD_RESET_RUN))
  # Export OPENOCD_CMD_RESET_RUN only to the reset target
  $(call target-export-variables,reset,OPENOCD_CMD_RESET_RUN)
endif

ifneq (,$(OPENOCD_CMD_RESET_HALT))
  # Export OPENOCD_CMD_RESET_HALT only to the flash targets
  $(call target-export-variables,flash,OPENOCD_CMD_RESET_HALT)
  $(call target-export-variables,flash-only,OPENOCD_CMD_RESET_HALT)
endif

OPENOCD_DEBUG_TARGETS = debug debugr debug-server debug-client

ifneq (,$(OPENOCD_DBG_EXTRA_CMD))
  # Export OPENOCD_DBG_EXTRA_CMD only to the flash/flash-only target
  $(call target-export-variables,$(OPENOCD_DEBUG_TARGETS),OPENOCD_DBG_EXTRA_CMD)
endif

OPENOCD_FLASH_TARGETS = flash flash-only flashr

ifneq (,$(IMAGE_OFFSET))
  # Export IMAGE_OFFSET only to the flash/flash-only target
  $(call target-export-variables,$(OPENOCD_FLASH_TARGETS),IMAGE_OFFSET)
endif

ifneq (,$(OPENOCD_POST_INIT_CMDS))
  # Export OPENOCD_POST_INIT_CMDS only to the flash/flash-only target
  $(call target-export-variables,$(OPENOCD_FLASH_TARGETS),OPENOCD_POST_INIT_CMDS)
endif

ifneq (,$(OPENOCD_PRE_VERIFY_CMDS))
  # Export OPENOCD_PRE_VERIFY_CMDS only to the flash/flash-only target
  $(call target-export-variables,$(OPENOCD_FLASH_TARGETS),OPENOCD_PRE_VERIFY_CMDS)
endif

ifneq (,$(OPENOCD_SKIP_VERIFY))
  # Export OPENOCD_SKIP_VERIFY only to the flash/flash-only target
  $(call target-export-variables,$(OPENOCD_FLASH_TARGETS),OPENOCD_SKIP_VERIFY)
endif

ifneq (,$(OPENOCD_PRE_FLASH_CMDS))
  # Export OPENOCD_PRE_FLASH_CMDS only to the flash/flash-only targets
  $(call target-export-variables,$(OPENOCD_FLASH_TARGETS),OPENOCD_PRE_FLASH_CMDS)
endif

ifneq (,$(OPENOCD_PRE_FLASH_CHECK_SCRIPT))
  # Export OPENOCD_PRE_FLASH_CHECK_SCRIPT only to the flash/flash-only targets
  $(call target-export-variables,$(OPENOCD_FLASH_TARGETS),OPENOCD_PRE_FLASH_CHECK_SCRIPT)
endif
