# Segger J-Link debug adapter
OPENOCD_ADAPTER_INIT ?= -c 'source [find interface/jlink.cfg]'

# default to SWD
OPENOCD_TRANSPORT ?= swd
