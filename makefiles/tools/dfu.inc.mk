DFU ?= dfu-util
FLASHER ?= $(DFU)
FLASHFILE ?= $(BINFILE)

FFLAGS ?= -D $(FLASHFILE) --reset $(DFU_ARGS)

