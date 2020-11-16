FLASHFILE ?= $(BINFILE)
FLASHER = $(RIOTTOOLS)/cc2538-bsl/cc2538-bsl.py
FFLAGS_OPTS ?=
PROG_BAUD ?= 500000	# default value in cc2538-bsl

FFLAGS  = $(if $(IMAGE_OFFSET), -a $(shell printf "0x%08x" $$(($(IMAGE_OFFSET) + $(ROM_START_ADDR)))))
FFLAGS += -p "$(PROG_DEV)" $(FFLAGS_OPTS) --write-erase -v -b $(PROG_BAUD) $(FLASHFILE)

RESET ?= $(FLASHER) -p "$(PROG_DEV)" $(FFLAGS_OPTS) -b $(PROG_BAUD)

ifeq ($(RIOTTOOLS)/cc2538-bsl/cc2538-bsl.py,$(FLASHER))
  FLASHDEPS += $(FLASHER)
endif
