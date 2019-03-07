RIOT_EDBG = $(RIOTTOOLS)/edbg/edbg
EDBG ?= $(RIOT_EDBG)
FLASHER ?= $(EDBG)
HEXFILE = $(BINFILE)
# Use USB serial number to select device when more than one is connected
# Use /dist/tools/usb-serial/list-ttys.sh to find out serial number.
#   Usage:
# export DEBUG_ADAPTER_ID="ATML..."
# BOARD=<board> make flash
ifneq (,$(DEBUG_ADAPTER_ID))
  EDBG_ARGS += --serial $(DEBUG_ADAPTER_ID)
endif

# Set offset according to IMAGE_OFFSET if it's defined
EDBG_ARGS += $(if $(IMAGE_OFFSET),--offset $(IMAGE_OFFSET))

FFLAGS ?= $(EDBG_ARGS) -t $(EDBG_DEVICE_TYPE) -b -v -p -f $(HEXFILE)

ifeq ($(RIOT_EDBG),$(FLASHER))
  FLASHDEPS += $(RIOT_EDBG)
endif

## A few commits ago edbg changed its behavior and now it resets the board
## twice. This makes some tests fail, as the start on the first reset, but
## shortly after the board is reset again.
## See: https://github.com/ataradov/edbg/issues/77
## Commenting this out causes RESET to be provided by OpenOCD instead.
# RESET ?= $(EDBG)
# RESET_FLAGS ?= $(EDBG_ARGS) -t $(EDBG_DEVICE_TYPE)
