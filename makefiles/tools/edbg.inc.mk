include $(RIOTMAKE)/tools/edbg-devices.inc.mk

# Edbg use a bin file for flashing
FLASHFILE ?= $(BINFILE)

EDBG ?= $(RIOTTOOLS)/edbg/edbg
FLASHDEPS += $(EDBG)

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
EDBG_ARGS += --target $(EDBG_DEVICE_TYPE)

EDBG_TARGETS = flash flash-only reset
# Export EDBG to required targets
$(call target-export-variables,$(EDBG_TARGETS),EDBG)

# Export EDBG_ARGS to required targets
$(call target-export-variables,$(EDBG_TARGETS),EDBG_ARGS)

# Set flasher and reset for the RIOT build system
FLASHER ?= $(RIOTTOOLS)/edbg/edbg.sh
ifeq (1, $(FAST_FLASH))
  FFLAGS ?= flash $(FLASHFILE)
else
  FFLAGS ?= flash_verify $(FLASHFILE)
endif

RESET ?= $(RIOTTOOLS)/edbg/edbg.sh
RESET_FLAGS ?= reset

# use openocd for debugging, must be included at the end so FLASHER/RESET
# variables are already set for edbg.
include $(RIOTMAKE)/tools/openocd.inc.mk
