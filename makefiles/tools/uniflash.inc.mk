# http://www.ti.com/tool/uniflash
FLASHFILE ?= $(ELFFILE)

UNIFLASH_CONFIG ?= $(BOARDDIR)/dist

UNIFLASH_CONFIG_CCXML ?= $(UNIFLASH_CONFIG)/$(CPU_MODEL)_$(XDEBUGGER).ccxml
UNIFLASH_CONFIG_DAT ?= $(UNIFLASH_CONFIG)/$(CPU_MODEL)_$(XDEBUGGER).dat
UNIFLASH_CONFIG_GDB ?= $(UNIFLASH_CONFIG)/$(CPU_MODEL)_gdb.conf

UNIFLASH_PATH ?= "UNIFLASH_PATH unconfigured"
# check which uniflash version is available, either 4.x or 3.x
ifneq ("$(wildcard $(UNIFLASH_PATH)/dslite.sh)","")
  FLASHER ?= $(UNIFLASH_PATH)/dslite.sh
  FFLAGS  = --config $(UNIFLASH_CONFIG_CCXML) $(FLASHFILE)
  # configure uniflash for resetting target
  # xds110reset path changed in version UniFlash v4.4.0
  # Try to detect the newest one and fallback to only 'xds110reset'
  _XDS110RESET_4_0_4_3 ?= $(UNIFLASH_PATH)/simplelink/gen2/bin/xds110reset
  _XDS110RESET ?= $(UNIFLASH_PATH)/simplelink/imagecreator/bin/xds110reset
  XDS110RESET ?= $(firstword $(wildcard $(_XDS110RESET) $(_XDS110RESET_4_0_4_3)) xds110reset)
  RESET ?= $(XDS110RESET)
else
  FLASHER = $(UNIFLASH_PATH)/uniflash.sh
  FFLAGS  = -ccxml $(UNIFLASH_CONFIG_CCXML) -program $(FLASHFILE)
  # configure uniflash for resetting target
  RESET ?= $(UNIFLASH_PATH)/uniflash.sh
  RESET_FLAGS ?= -ccxml $(UNIFLASH_CONFIG_CCXML) -reset
endif

CCS_PATH ?= "CCS_PATH unconfigured"
# configure the debug server
DEBUGSERVER ?= $(CCS_PATH)/ccs/ccs_base/common/uscif/gdb_agent_console
DEBUGSERVER_FLAGS ?= -p 3333 $(UNIFLASH_CONFIG_DAT)

# configure the debugging tool
DEBUGGER ?= $(PREFIX)gdb
DEBUGGER_FLAGS ?= -x $(UNIFLASH_CONFIG_GDB) $(ELFFILE)
