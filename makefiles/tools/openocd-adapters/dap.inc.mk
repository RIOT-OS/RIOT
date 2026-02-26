# CMSIS DAP debug adapter
OPENOCD_ADAPTER_INIT ?= -c 'source [find interface/cmsis-dap.cfg]'

# default to SWD
OPENOCD_TRANSPORT ?= swd
