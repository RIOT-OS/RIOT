DFU ?= dfu-util
FLASHER ?= $(DFU)
FLASHFILE ?= $(BINFILE)

DFU_ALT ?= 0
ROM_OFFSET ?= 0

_ROM_ADDR_WITH_OFFSET ?= $(shell printf "0x%x" $$(($(ROM_START_ADDR) + $(ROM_OFFSET))))
FLASH_ADDR ?= $(if $(ROM_OFFSET),$(_ROM_ADDR_WITH_OFFSET),$(ROM_START_ADDR))

# Optional flasher flags
FFLAGS_OPTS ?=

FFLAGS ?= --device $(DFU_USB_ID) \
          --alt $(DFU_ALT) \
          --dfuse-address $(FLASH_ADDR):leave \
          --download $(FLASHFILE) \
          $(FFLAGS_OPTS)
