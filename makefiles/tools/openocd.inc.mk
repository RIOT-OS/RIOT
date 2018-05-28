export FLASHER ?= $(RIOTTOOLS)/openocd/openocd.sh
export DEBUGGER = $(RIOTTOOLS)/openocd/openocd.sh
export DEBUGSERVER = $(RIOTTOOLS)/openocd/openocd.sh
export RESET ?= $(RIOTTOOLS)/openocd/openocd.sh

export FFLAGS ?= flash
export DEBUGGER_FLAGS ?= debug
export DEBUGSERVER_FLAGS ?= debug-server
export RESET_FLAGS ?= reset

ifneq (,$(DEBUG_ADAPTER))
  include $(RIOTMAKE)/tools/openocd-adapters/$(DEBUG_ADAPTER).inc.mk
endif
