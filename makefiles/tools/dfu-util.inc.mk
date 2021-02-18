DFU ?= dfu-util
FLASHER ?= $(DFU)
FLASHFILE ?= $(BINFILE)

DFU_ALT ?= 0
DFU_USB_ID ?= ${USB_VID}:${USB_PID}
ROM_OFFSET ?= 0

_ROM_ADDR_WITH_OFFSET ?= $(shell printf "0x%x" $$(($(ROM_START_ADDR) + $(ROM_OFFSET))))
FLASH_ADDR ?= $(if $(ROM_OFFSET),$(_ROM_ADDR_WITH_OFFSET),$(ROM_START_ADDR))
DFU_USE_DFUSE ?= 0
# Optional flasher flags
DFU_UTIL_FLAGS_OPTS ?=

FFLAGS ?= --device $(DFU_USB_ID) \
          --alt $(DFU_ALT) \
          --download $(FLASHFILE) \
          $(DFU_UTIL_FLAGS_OPTS)

ifeq ($(DFU_USE_DFUSE),1)
  FFLAGS += --dfuse-address $(FLASH_ADDR):leave
endif
