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
endif

OPENOCD_CONFIG ?= $(BOARDSDIR)/$(BOARD)/dist/openocd.cfg

# Export OPENOCD_CONFIG to required targets
OPENOCD_TARGETS = debug% flash% reset
$(call target-export-variables,$(OPENOCD_TARGETS),OPENOCD_CONFIG)

# Export OPENOCD_ADAPTER_INIT to required targets
$(call target-export-variables,$(OPENOCD_TARGETS),OPENOCD_ADAPTER_INIT)

# Export OPENOCD_RESET_USE_CONNECT_ASSERT_SRST to required targets
$(call target-export-variables,$(OPENOCD_TARGETS),OPENOCD_RESET_USE_CONNECT_ASSERT_SRST)
