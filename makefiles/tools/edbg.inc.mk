RIOT_EDBG = $(RIOTTOOLS)/edbg/edbg
EDBG ?= $(RIOT_EDBG)
FLASHER ?= $(EDBG)
FLASHFILE ?= $(BINFILE)
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

FFLAGS ?= $(EDBG_ARGS) -t $(EDBG_DEVICE_TYPE) -b -v -p -f $(FLASHFILE)

ifeq ($(RIOT_EDBG),$(FLASHER))
  FLASHDEPS += $(RIOT_EDBG)
endif
RESET ?= $(EDBG)
RESET_FLAGS ?= $(EDBG_ARGS) -t $(EDBG_DEVICE_TYPE)
