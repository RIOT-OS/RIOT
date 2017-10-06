# CMSIS DAP debug adapter
OPENOCD_ADAPTER_INIT ?= -c 'source [find interface/cmsis-dap.cfg]'
# Add serial matching command, only if DEBUG_ADAPTER_ID was specified
ifneq (,$(DEBUG_ADAPTER_ID))
  OPENOCD_ADAPTER_INIT += -c 'cmsis_dap_serial $(DEBUG_ADAPTER_ID)'
endif
export OPENOCD_ADAPTER_INIT
