FLASHER ?= nrfjprog

#nrfjprog only support hexfile
FLASHFILE ?= $(HEXFILE)
FFLAGS ?= --program $(FLASHFILE)
