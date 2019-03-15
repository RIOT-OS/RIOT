# http://www.ti.com/tool/uniflash
export UNIFLASH_PATH ?= "UNIFLASH_PATH unconfigured"
# check which uniflash version is available, either 4.x or 3.x
ifneq ("$(wildcard $(UNIFLASH_PATH)/dslite.sh)","")
  export FLASHER ?= $(UNIFLASH_PATH)/dslite.sh
  export FFLAGS  = --config $(RIOTBOARD)/$(BOARD)/dist/$(CPU_MODEL)_$(XDEBUGGER).ccxml $(ELFFILE)
  # configure uniflash for resetting target
  export RESET = $(UNIFLASH_PATH)/simplelink/gen2/bin/xds110reset
  export RESET_FLAGS
else
  export FLASHER = $(UNIFLASH_PATH)/uniflash.sh
  export FFLAGS  = -ccxml $(RIOTBOARD)/$(BOARD)/dist/$(CPU_MODEL)_$(XDEBUGGER).ccxml -program $(ELFFILE)
  # configure uniflash for resetting target
  export RESET = $(UNIFLASH_PATH)/uniflash.sh
  export RESET_FLAGS = -ccxml $(RIOTBOARD)/$(BOARD)/dist/$(CPU_MODEL)_$(XDEBUGGER).ccxml -reset
endif
# configure the debug server
export DEBUGSERVER = $(UNIFLASH_PATH)/ccs_base/common/uscif/gdb_agent_console
export DEBUGSERVER_FLAGS = -p 3333 $(RIOTBOARD)/$(BOARD)/dist/$(CPU_MODEL)_$(XDEBUGGER).dat

# configure the debugging tool
export DEBUGGER = $(PREFIX)gdb
export DEBUGGER_FLAGS = -x $(RIOTBOARD)/$(BOARD)/dist/$(CPU_MODEL)_gdb.conf $(ELFFILE)
