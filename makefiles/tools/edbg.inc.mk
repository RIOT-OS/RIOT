include $(RIOTMAKE)/tools/edbg-devices.inc.mk

RIOT_EDBG = $(RIOTTOOLS)/edbg/edbg
EDBG ?= $(RIOT_EDBG)
FLASHER ?= $(EDBG)
FLASHFILE ?= $(BINFILE)
# Use USB serial number to select device when more than one is connected
# Use /dist/tools/usb-serial/list-ttys.sh to find out serial number.
#   Usage:
# DEBUG_ADAPTER_ID="ATML..."
# BOARD=<board> make flash
ifneq (,$(DEBUG_ADAPTER_ID))
  EDBG_ARGS += --serial $(DEBUG_ADAPTER_ID)
endif

# Set offset according to IMAGE_OFFSET if it's defined
EDBG_ARGS += $(if $(IMAGE_OFFSET),--offset $(IMAGE_OFFSET))

FFLAGS ?= $(EDBG_ARGS) --target $(EDBG_DEVICE_TYPE) --verbose \
                       --file $(FLASHFILE)

ifeq ($(RIOT_EDBG),$(FLASHER))
  FLASHDEPS += $(RIOT_EDBG)
endif
RESET ?= $(EDBG)
RESET_FLAGS ?= $(EDBG_ARGS) --target $(EDBG_DEVICE_TYPE)

define edbg-flash-recipe
  $(call check_cmd,$(FLASHER),Flash program)
  $(FLASHER) $(FFLAGS) --verify || $(FLASHER) $(FFLAGS) --verify --program
endef

flash-recipe = $(edbg-flash-recipe)
