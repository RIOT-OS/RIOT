# Select default FTDI debug adapter board
OPENOCD_FTDI_ADAPTER ?= tigard

# FTDI debug adapter interface
OPENOCD_ADAPTER_INIT ?= -c 'source [find interface/ftdi/$(OPENOCD_FTDI_ADAPTER).cfg]'

# default to SWD
OPENOCD_TRANSPORT ?= swd

# Add serial matching command, only if DEBUG_ADAPTER_ID was specified
ifneq (,$(DEBUG_ADAPTER_ID))
  OPENOCD_ADAPTER_INIT += -c 'ftdi_serial $(DEBUG_ADAPTER_ID)'
endif
