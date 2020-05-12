# ST-Link debug adapter
# Use st-link v2-1 by default
STLINK_VERSION ?= 2-1

# Use STLINK_VERSION to select which stlink version is used
OPENOCD_ADAPTER_INIT ?= \
  -c 'source [find interface/stlink-v$(STLINK_VERSION).cfg]' \
  -c 'transport select hla_swd'
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

# if no openocd specific configuration file, check for default locations:
# 1. Using the default dist/openocd.cfg (automatically set by openocd.sh)
# 2. Using the common cpu specific config file
ifeq (,$(OPENOCD_CONFIG))
  # if no openocd default configuration is provided by the board,
  # use the STM32 common one
  ifeq (0,$(words $(wildcard $(BOARDDIR)/dist/openocd.cfg)))
    OPENOCD_CONFIG = $(RIOTBASE)/boards/common/stm32/dist/$(CPU).cfg
  endif
endif
