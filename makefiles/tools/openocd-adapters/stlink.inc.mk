# ST-Link debug adapter

# Set the default version for the old OpenOCD versions
STLINK_VERSION ?= 2-1

# Determine which configuration file to use for OpenOCD.
# Also pass the correct version of the ST-Link adapter to the script.
OPENOCD_ADAPTER_INIT ?= -c 'set stlink_version $(STLINK_VERSION);source $(RIOTBASE)/dist/tools/openocd/adapters/stlink.cfg'

# If swd / jtag is selected by the board, prefix it with hla_
ifneq (,$(filter swd jtag,$(OPENOCD_TRANSPORT)))
  OPENOCD_TRANSPORT := hla_$(OPENOCD_TRANSPORT)
endif

# All ST-Link adapters support hla_swd, so use that for simplicity
OPENOCD_TRANSPORT ?= hla_swd

# Add serial matching command, only if DEBUG_ADAPTER_ID was specified
ifneq (,$(DEBUG_ADAPTER_ID))
  OPENOCD_ADAPTER_INIT += -c 'hla_serial $(DEBUG_ADAPTER_ID)'
endif

# Some stlink clones cannot signal reset properly,
# In this case, use SRST=none
ifneq (,$(SRST))
  #Change the adapter init to it
  # Use STLINK_VERSION to select which stlink version is used
  OPENOCD_ADAPTER_INIT += \
    -c 'set SRST_OPT $(SRST)'
endif
