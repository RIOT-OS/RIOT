# ST-Link debug adapter

ifeq (,$(STLINK_VERSION))
    # Use stlink.cfg by default
    STLINK_CFG = stlink.cfg
else
    # Use STLINK_VERSION to select which stlink version is used.
    # deprecated in OpenOCD since Jan 26, 2017
    STLINK_CFG = stlink-v$(STLINK_VERSION).cfg
endif

OPENOCD_ADAPTER_INIT ?= \
  -c 'source [find interface/$(STLINK_CFG)]' \
  -c 'transport select hla_swd'
# Add serial matching command, only if DEBUG_ADAPTER_ID was specified
ifneq (,$(DEBUG_ADAPTER_ID))
  OPENOCD_ADAPTER_INIT += -c 'hla_serial $(DEBUG_ADAPTER_ID)'
endif
export OPENOCD_ADAPTER_INIT

# if no openocd specific configuration file, check for default locations:
# 1. Using the default dist/openocd.cfg (automatically set by openocd.sh)
# 2. Using the common cpu specific config file
ifeq (,$(OPENOCD_CONFIG))
  # if no openocd default configuration is provided by the board,
  # use the STM32 common one
  ifeq (0,$(words $(wildcard $(RIOTBOARD)/$(BOARD)/dist/openocd.cfg)))
    export OPENOCD_CONFIG := $(RIOTBASE)/boards/common/stm32/dist/$(CPU).cfg
  endif
endif
